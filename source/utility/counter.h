/*!
 * @file
 * 
 * @brief カウンタークラス
 * 
 * yaneurao 氏が作成した CRootCounter 等を改変しました
 * 
 * @author yaneurao
 * 
 * @since 2006/02/03
 */
#pragma once

__inline int ToInt(int n){
#if 0
	if (n > 32767){
		return 32767;
	}else if (n < -32768){
		return -32768;
	}
#endif
	return n;
}

/**
 * @brief ループカウンタ
 * 
 * 初期値(_start)・終了値(_end)、そして必要ならば増加値(_step)を設定します。<br>
 * そのあと、inc関数を実行すると、1ずつ増えます。(_stepが設定されていれば、
 * その数だけ増えます)　そして、加算した結果、_end以上になると自動的に
 * _startに戻ります。(_endにはなりません)<br>
 * <br>
 * reset関数は、カウンタ値を初期値_startに戻します。<br>
 * あるいは、setInit関数でreset後の初期値が設定されていれば、その値にします。<br>
 * <br>
 * また、int型との相互変換が可能ですので、あたかもint型の変数であるかのように
 * 使えます。<br>
 * <br>
 * 例）<br>
 * @code
 * RootCounter r;
 * r.set (0, 256 , -5);
 * // _start == 0 , _end == 256というようにカウンタを設定する
 * r = 128;
 * @endcode
 * 
 * この状態で、r++; を５回実行すると５回目で r == 129になる。<br>
 * r--; を５回実行すると５回目で r == 127になる。<br>
 * <br>
 * また、_start≦_endでなくて良いのです。
 * つまり、加算(inc / ++)では、_end方向へインクリメントします。
 * 減算(dec / --)では、_start方向へインクリメントします。
 * 
 * また、_stepがマイナスである場合は、
 * その絶対値回数のincメンバ関数の呼び出しによって、1だけ_end方向に進みます。
*/
class RootCounter{
public:
	//	stepは一回の増分の絶対値。マイナスは1/stepの意味
	//	start≦endでなくて良い
	void set(int start , int end , int step = 1);
	void setStep(int step){ _step = ToInt(step); }
	void setStart(int start){ _start = ToInt(start); }
	void setEnd(int end){ _end = ToInt(end); }
	void setOffset(int offset){ _offset = ToInt(offset); }
	void setSize(int size);
	
	///	取得
	int getStep() const{ return _step; }
	int getStart() const{ return _start; }
	int getEnd() const{ return _end; }
	int getOffset() const{ return _offset; }
	int getPos() const{ return (_rootCount-_offset); }
	int getSize() const{ return _size; }
	
	///	カウンタのリセット
	void reset();
	
	//	property..
	bool isBegin() const{ return _rootCount == _start; }
	bool isEnd() const{ return _rootCount == _end; }
	
	RootCounter();
	RootCounter(int end);
	RootCounter(int start ,int end , int step = 1);
	
	//	intとの相互変換
	operator int() const{ return _rootCount; }
	int operator=(int n){ _rootCount = ToInt(n); return n; }
	int get() const{ return _rootCount; }
	
	//	カウンタのインクリメント(終端まで達すると、そこで停止する)
	void inc(){ _inc(true); }
	void dec(){ _inc(false); }
	
	//	加算（＝end方向へインクリメント）／減算（＝start方向へのインクリメント）
	RootCounter& operator++()
		{ inc(); return (*this); }
	RootCounter operator++(int)
		{ RootCounter _tmp = *this; inc(); return _tmp; }
	RootCounter& operator--()
		{ dec(); return (*this); }
	RootCounter operator--(int)
		{ RootCounter _tmp = *this; dec(); return _tmp; }
protected:
	void	_inc(bool bAdd = true);
	
	int		_rootCount;
	int		_start;
	int		_end;
	int		_step;
	int		_rate;	//	step<0のときは、ｎ回のinc()で+1される
	int		_size;
	int		_offset;
};

/**
 * @brief 飽和カウンタ
 * 
 * RootCounter のバリエーション。<br>
 * 「カウンタを++していき、終了値になったときに、そこでカウンタは停止する」という部分だけが異なる。
 */
class SaturationCounter{
public:
	//	stepは一回の増分の絶対値。マイナスは1/stepの意味
	//	start≦endでなくて良い
	void set(int start , int end , int step = 1);
	void setStep(int step){ _step = ToInt(step); }
	void setStart(int start){ _start = ToInt(start); }
	void setEnd(int end){ _end = ToInt(end); }
	
	///	取得
	int getStep() const{ return _step; }
	int getStart() const{ return _start; }
	int getEnd() const{ return _end; }
	
	///	カウンタのリセット
	void reset();
	
	///	property..
	bool isBegin() const{ return _rootCount == _start; }
	bool isEnd() const{ return _rootCount == _end; }

	SaturationCounter();
	SaturationCounter(int end);
	SaturationCounter(int start ,int end , int step = 1);
	
	//	intとの相互変換
	operator int() const{ return _rootCount; }
	int operator=(int n){ _rootCount = ToInt(n); return n; }
	int get() const{ return _rootCount; }
	
	//	カウンタのインクリメント(終端まで達すると、そこで停止する)
	void inc(){ _inc(true); }
	void dec(){ _inc(false); }
	
	//	加算（＝end方向へインクリメント）／減算（＝start方向へのインクリメント）
	SaturationCounter& operator++()
		{ inc(); return (*this); }
	SaturationCounter operator++(int)
		{ SaturationCounter _tmp = *this; inc(); return _tmp; }
	SaturationCounter& operator--()
		{ dec(); return (*this); }
	SaturationCounter operator--(int)
		{ SaturationCounter _tmp = *this; dec(); return _tmp; }
protected:
	void	_inc(bool bAdd = true);
	
	int		_rootCount;
	int		_start;
	int		_end;
	int		_step;
	int		_rate;	//	step<0のときは、ｎ回のinc()で+1される
};

/**
 * @brief 内分カウンタを提供します。
 * 
 * setメンバ関数で、初期値(start)・終了値(end)、
 * そして分割数(frames)を設定します。<br>
 * <br>
 * そうすれば、このカウンタはstartから始まり、
 * frame回のincメンバ関数の呼び出しによってendに到達するような
 * 内分カウンタとなります。
 * 
 * また、incは、operator++としても定義してあります。
 * incの逆操作であるdecもあり、それはoperator--としても定義してあります。
 */
class InteriorCounter{
public:
	InteriorCounter();
	
	///	intとの相互変換
	operator int () const{ return _now; }
	int operator=(int n){ _now = _start = ToInt(n); _framesNow = 0; return n; }
	
	void inc();		///	加算
	InteriorCounter& operator++()
		{ inc(); return (*this); }
	InteriorCounter operator++(int)
		{ InteriorCounter _tmp = *this; inc(); return _tmp; }
	
	void dec();		///	減算
	InteriorCounter& operator--()
		{ dec(); return (*this); }
	InteriorCounter operator--(int)
		{ InteriorCounter _tmp = *this; dec(); return _tmp; }
	
	int get() const{ return _now; }
	/**
	 * @brief 初期値(@a start)・終了値(@a end)、そして分割数(@a frames)を設定します。
	 */
	void set(int start , int end , int frames);
	
	//!	現在の値を（一時的に）変更する。次のinc/decで、正常な値に戻る
//	void set(int now){ *this = now; }
	void set(int now){ _now = ToInt(now); }
	
	void reset(){ set(getStart() , getEnd() , getFrame()); }

//	bool isBegin() const{ return _now == _start; }
//	bool isEnd() const{ return _now == _end; }
	bool isBegin() const{ return _framesNow == 0; }
	bool isEnd() const{
		return _framesNow == _frames;
	}

	//	property
	int		getFrame() const{ return _frames; }
	int		getStart() const{ return _start;}
	int		getEnd() const{ return _end;}
	
	//! 現在のフレーム数(incをした回数)の取得。
	int		getFrameNow() const { return _framesNow; }
	
protected:
	int		_now;			//	現在の値
	int		_start;			//	初期値
	int		_end;			//	終了値
	int		_frames;		//	フレーム分割数（終了値になるまで何回incをすればいいのか）
	int		_framesNow;		//	現在、何フレーム目か？
};

class LoopCounter{
public:
	void set(int start , int end , int step = 1){
		_counter.set(start , end , step);
		_bUp = true;
	}
	void reset(){
		if (isUp()){
			set(_counter.getStart() , _counter.getEnd() , _counter.getStep());
		}else{
			set(_counter.getEnd() , _counter.getStart() , _counter.getStep());
			_bUp = true;
		}
	}

	//	intとの相互変換
	operator int() const{ return get(); }
	int get() const{ return _counter.get(); }
	int operator=(int n){ return _counter.operator=(n); }
	
	///	取得
	int getStep() const{ return _counter.getStep(); }
	int getStart() const{ return isUp() ? _counter.getStart() : _counter.getEnd(); }
	int getEnd() const{ return isUp() ? _counter.getEnd() : _counter.getStart(); }
	
	///	property..
	bool isBegin() const{ return isUp() ? _counter.isBegin() : _counter.isEnd(); }
	bool isEnd() const{ return isBegin(); }

	///	加算
	void inc(){
		++_counter;
		if (_counter.isEnd()){
			int s = _counter.getStart();
			int e = _counter.getEnd();
			_counter.setStart(e);
			_counter.setEnd(s);
			_bUp = !_bUp;
		}
	}
	LoopCounter& operator++()
		{ inc(); return (*this); }
	LoopCounter operator++(int)
		{ LoopCounter _tmp = *this; inc(); return _tmp; }
	
	///	減算
	void dec(){
		--_counter;
		if (_counter.isBegin()){
			int s = _counter.getStart();
			int e = _counter.getEnd();
			_counter.setStart(e);
			_counter.setEnd(s);
			_bUp = !_bUp;
		}
	}
	LoopCounter& operator--()
		{ dec(); return (*this); }
	LoopCounter operator--(int)
		{ LoopCounter _tmp = *this; dec(); return _tmp; }

	bool isUp() const{ return _bUp; }
protected:
	SaturationCounter	_counter;
	bool				_bUp;
};
