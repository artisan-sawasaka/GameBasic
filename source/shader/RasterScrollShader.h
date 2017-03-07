#pragma once

#include "Shader.h"
#include "render/Color.h"
#include <string>

/*!
 * @brief ���X�^�[�X�N���[��
 */
class RasterScrollShader : public Shader
{
public :
	RasterScrollShader();
	~RasterScrollShader();

	virtual bool LoadFile(const char* path) { return false; }

	/*!
	 * @brief �c�̈ړ��l��ݒ�
	 */
	void SetOffset(float value);
	/*!
	 * @brief ���̈ړ��l��ݒ�
	 */
	void SetPeriod(float value);
	/*!
	 * @brief 1�J�[�u�ӂ�̑傫����ݒ�
	 */
	void SetAmplitude(float value);
	/*!
	 * @brief �J���[�̐ݒ�
	 */
	void SetColor(const Color& color);

	/*!
	 * @brief �c�̈ړ��l���擾
	 */
	float GetOffset() const { return offset_; }
	/*!
	 * @brief ���̈ړ��l��ݒ�
	 */
	float GetPeriod() const { return period_; }
	/*!
	 * @brief 1�J�[�u�ӂ�̑傫����ݒ�
	 */
	float GetAmplitude() const { return amplitude_; }

private :
	D3DXHANDLE offset_handle_;
	D3DXHANDLE period_handle_;
	D3DXHANDLE amplitude_handle_;
	D3DXHANDLE diffuse_handle_;
	
	float offset_;
	float period_;
	float amplitude_;
};