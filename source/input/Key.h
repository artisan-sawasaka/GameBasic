/*!
 * @file
 *
 * @brief �L�[�Ǘ��N���X��񋟂��܂��B
 *
 * @copyright
 */
#pragma once

/*!
 * @brief �L�[�Ǘ��N���X
 */
class Key
{
public :
	/*!
	 * @brief �L�[�̃N���A
	 */
	void Clear();

	/*!
	 * @brief ��������
	 */
	void Down(unsigned char key);

	/*!
	 * @brief ��������
	 */
	void Up(unsigned char key);

	/*!
	 * @brief �L�[�̍X�V
	 */
	void Update();

	/*!
	 * @brief �L�[�̉�������ςȂ����擾
	 */
	bool IsPress(unsigned char key) const;

	/*!
	 * @brief �L�[�̉����������擾
	 */
	bool IsTrg(unsigned char key) const;

	/*!
	 * @brief �L�[�̗����������擾
	 */
	bool IsRelease(unsigned char key) const;

	/*!
	 * @brief �L�[���s�[�g
	 */
	bool IsRepeat(unsigned char key) const;

	static Key* GetInstance() {
		static Key v;
		return &v;
	}

private :
	Key();

	struct Info {
		bool key;
		bool key_back;
		bool key_temp;
		int key_count;
	};
	Info infos_[256];
};