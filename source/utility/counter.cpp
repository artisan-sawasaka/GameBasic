#include "Counter.h"

#if		defined(WIN32)
typedef	signed __int64 INT64;
#elif	defined(PLATFORM_PSP)
typedef	signed long long INT64;
#elif	defined(PLATFORM_PS3)
typedef	signed long long INT64;
#endif

RootCounter::RootCounter() {
	set(0 , 0x7fffffff , 1);
	reset();
}

RootCounter::RootCounter(int end) {
	set(0 , end , 1);
	reset();
}

RootCounter::RootCounter(int start ,int end , int step) {
	set(start , end , step);
	reset();
}

void RootCounter::set(int start , int end , int step) {
	_start = ToInt(start);
	_end = _size = ToInt(end);
	_step = ToInt(step);
	reset();
}

void RootCounter::setSize(int size)
{
	_size = ToInt(size);
	if ( _size > _end ) _size = _end;
	_offset = _rootCount;
	if ( _offset > (_end - _size) ) _offset = _end - _size;
}

void RootCounter::reset() {
	_rootCount = _start;
	_rate = 0;
	_offset = 0;
}

void RootCounter::_inc(bool bAdd) {
	bool bInc = (_start > _end) ^ bAdd; // 逆方向カウンタ？
	if (bInc) {
	//	インクリメント
		if (_step > 0) {
		//	整数インクリメント
			_rootCount += _step;
		} else {
		//	分数インクリメント
			_rate++; if (_rate >= -_step) { _rate = 0; _rootCount++; }
		}
		//	サチュレートしたのか？
		int max = _start < _end ? _end : _start;
		if (_rootCount >= max) {
			int min = _start < _end ? _start : _end;
			_rootCount = min;
			//		CRootCounterとはここ↑が違うだけ
		}
		// オフセット
		if ( (_rootCount-_offset) >= _size ) _offset++;
		if ( _rootCount == 0 ) _offset = 0;
	} else {
	//	デクリメント
		if (_step > 0) {
		//	整数デクリメント
			_rootCount -= _step;
		} else {
		//	分数デクリメント
			_rate--; if (_rate <= _step) { _rate = 0; _rootCount--; }
			//	⇒　_rate++でないことに注意。
			//	++のあと--して、数の整合性がとれなくてはならない
			//	かつ、nStep<0のとき最初の１回目の--でRootCounterが
			//	1減ってはいけない。よってこういう実装になる
		}
		//	サチュレートしたのか？
		int min = _start < _end ? _start : _end;
		if (_rootCount < min) {
			int max = _start < _end ? _end : _start;
			if (_step > 0) {
				_rootCount = max - _step;
			} else {
				_rootCount = max - 1;
			}
			//		CRootCounterとはここ↑が違うだけ
		}
		// オフセット
		if ( (_rootCount-_offset) < 0 ) _offset--;
		if ( _rootCount == (_end-1) ) _offset = _end - _size;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

SaturationCounter::SaturationCounter() {
	set(0 , 0x7fffffff , 1);
	reset();
}

SaturationCounter::SaturationCounter(int end) {
	set(0 , end , 1);
	reset();
}

SaturationCounter::SaturationCounter(int start ,int end , int step) {
	set(start , end , step);
	reset();
}

void SaturationCounter::set(int start , int end , int step) {
	_start = ToInt(start);
	_end = ToInt(end);
	_step = ToInt(step);
	reset();
}

void SaturationCounter::reset() {
	_rootCount = _start;
	_rate = 0;
}

void SaturationCounter::_inc(bool bAdd) {
	bool bInc = (_start > _end) ^ bAdd; // 逆方向カウンタ？
	if (bInc) {
	//	インクリメント
		if (_step > 0) {
		//	整数インクリメント
			_rootCount += _step;
		} else {
		//	分数インクリメント
			_rate++; if (_rate >= -_step) { _rate = 0; _rootCount++; }
		}
		//	サチュレートしたのか？
		int max = _start < _end ? _end : _start;
		if (_rootCount > max) _rootCount = max;
			//		CRootCounterとはここ↑が違うだけ
	} else {
	//	デクリメント
		if (_step > 0) {
		//	整数デクリメント
			_rootCount -= _step;
		} else {
		//	分数デクリメント
			_rate--; if (_rate <= _step) { _rate = 0; _rootCount--; }
			//	⇒　_rate++でないことに注意。
			//	++のあと--して、数の整合性がとれなくてはならない
			//	かつ、nStep<0のとき最初の１回目の--でRootCounterが
			//	1減ってはいけない。よってこういう実装になる
		}
		//	サチュレートしたのか？
		int min = _start < _end ? _start : _end;
		if (_rootCount < min) _rootCount = min;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

InteriorCounter::InteriorCounter() {
	_start		= 0;
	_end		= 0;
	_now		= 0;
	_frames		= 0;
	_framesNow	= 0;
}

void InteriorCounter::set(int start , int end , int frames) {
//	WARNING(nFrames == 0,"CInteriorCounter::SetでnFrames == 0");
	// ゼロ除算を回避する
	if (frames == 0) frames = 1;
	
	_start		= ToInt(start);
	_end		= ToInt(end);
	_now		= ToInt(start);
	_frames		= ToInt(frames);
	_framesNow	= 0;
}

void InteriorCounter::inc() {
	//	カウンタは終了値か？
	if (_framesNow >= _frames) {
		_now = _end;
		return;
	}
	_framesNow++;
	//	内分処理
	_now =  _start + static_cast<int>(static_cast<INT64>(_framesNow) * (_end - _start) / _frames);
}

void InteriorCounter::dec() {
	//	カウンタは初期値か？
	if (_framesNow == 0) return;
	_framesNow--;
	//	内分処理
	_now =  _start + static_cast<int>(static_cast<INT64>(_framesNow) * (_end - _start) / _frames);
}
