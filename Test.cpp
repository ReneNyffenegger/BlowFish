
#include <iostream>
#include <fstream>
#include "Blowfish.h"

using namespace std;

//Function to convert unsigned char to string of length 2
void Char2Hex(const unsigned char ch, char* szHex)
{
	unsigned char byte[2];
	byte[0] = ch/16;
	byte[1] = ch%16;
	for(int i=0; i<2; i++)
	{
		if(byte[i] >= 0 && byte[i] <= 9)
			szHex[i] = '0' + byte[i];
		else
			szHex[i] = 'A' + byte[i] - 10;
	}
	szHex[2] = 0;
}

//Function to convert string of length 2 to unsigned char
void Hex2Char(const char* szHex, unsigned char& rch)
{
	rch = 0;
	for(int i=0; i<2; i++)
	{
		if(*(szHex + i) >='0' && *(szHex + i) <= '9')
			rch = (rch << 4) + (*(szHex + i) - '0');
		else if(*(szHex + i) >='A' && *(szHex + i) <= 'F')
			rch = (rch << 4) + (*(szHex + i) - 'A' + 10);
		else
			break;
	}
}    

//Function to convert string of unsigned chars to string of chars
void CharStr2HexStr(const unsigned char* pucCharStr, char* pszHexStr, int iSize)
{
	int i;
	char szHex[3];
	pszHexStr[0] = 0;
	for(i=0; i<iSize; i++)
	{
		Char2Hex(pucCharStr[i], szHex);
		strcat(pszHexStr, szHex);
	}
}

//Function to convert string of chars to string of unsigned chars
void HexStr2CharStr(const char* pszHexStr, unsigned char* pucCharStr, int iSize)
{
	int i;
	unsigned char ch;
	for(i=0; i<iSize; i++)
	{
		Hex2Char(pszHexStr+2*i, ch);
		pucCharStr[i] = ch;
	}
}

std::string encrypt_text(std::string key, char* _text, int _size)
{
	CBlowFish fish((unsigned char*)key.c_str(),key.length());
	int cnt = 0;
	unsigned char buff[8];
	int i = 0;

	std::ostringstream oss;
	int my_size = 0;
	while (i < _size)
	{
		buff[cnt++] = _text[i];
		if (cnt == 8)
		{
			//Encrypt(buff);
			fish.Encrypt((unsigned char*)buff, 8);

			for (int j = 0; j < 8; j++)
			{
				oss << buff[j];
				my_size++;
			}
			cnt = 0;
		}
		i++;
	}
	/* An EOF is reached before counting 8 chars. Pad remaining with 0s and encrypt */
	if (cnt != 0)
	{
		if (buff[cnt - 1] == 10)
			cnt--;
		if (cnt)
		{
			for (int i = cnt; i < 8; i++)
			{
				buff[i] = 0;
			}
			fish.Encrypt((unsigned char*)buff, 8);
			for (int j = 0; j < 8; j++)
			{
				oss << buff[j];
				my_size++;
			}
		}
	}

	return oss.str();
}

std::string decrypt_text(std::string key, char* _text, int _size)
{
	CBlowFish fish((unsigned char*)key.c_str(),key.length());
	int cnt = 0;
	int i = 0;
	unsigned char buff[8];
	std::ostringstream oss2;

	while (i < _size)
	{
		buff[cnt++] = _text[i];

		if (cnt == 8)
		{
			//Encrypt(buff);
			fish.Decrypt((unsigned char*)buff, 8);

			for (int j = 0; j < 8; j++)
			{
				oss2 << buff[j];
			}
			cnt = 0;
		}
		i++;
	}
	/* An EOF is reached before counting 8 chars. Pad remaining with 0s and encrypt */
	if (cnt != 0)
	{
		if (buff[cnt - 1] == 10)
			cnt--;
		if (cnt)
		{
			fish.Decrypt((unsigned char*)buff, 8);

			for (int i = cnt; i < 8; i++)
			{
				buff[i] = 0;
			}

			fish.Decrypt((unsigned char*)buff, 8);
			for (int j = 0; j < 8; j++)
			{
				oss2 << buff[j];
			}
		}
	}

	return oss2.str();
}

void main()
{

	// we read the file
	ifstream file_ ("text_to_encrypt.txt", ios::in|ios::binary|ios::ate);
	int _size = file_.tellg();
	char *_memblock = new char [_size];
	file_.seekg (0, ios::beg);
	file_.read (_memblock, _size);
	file_.close();
	
	//then we encrypt the file and free memory
	std::string enc_text = encrypt_text("zaxuhe_test",_memblock,_size);
	delete[] _memblock;


	//we save the resulting encrypted file
	std::ofstream myfile;
	myfile.open ("encrypted_test.bin",ios::binary);
	myfile.write((const char*)enc_text.c_str(),enc_text.length());
	myfile.close();

	//we also save the file but instead of saving it in binary we save it in hex
	//you can check this using the key at http://www.everpassword.com/blowfish-encryptor

	ofstream outfile ("encrypted_test_text.txt");
	char *hex_str = new char[enc_text.size()*2+2];
	CharStr2HexStr((unsigned char*)enc_text.c_str(),hex_str,enc_text.size());
	outfile << hex_str;
	outfile.close();

	delete[] hex_str;

	//we open the file
	ifstream file ("encrypted_test.bin", ios::in|ios::binary);

	if (file.is_open())
	{
		//we load file data
		file.seekg(0, ios::end); // position get-ptr 0 bytes from end
		int size = file.tellg();
		char *memblock = new char [size+1];
		file.seekg (0, ios::beg);
		//we read the file
		file.read (memblock, size);
		memblock[size] = '\0';
		file.close();

		//we decrypt the file :)
		std::string file_out = decrypt_text("zaxuhe_test",memblock,size);
		std::cout << "Decrypt: " << file_out;
		//std::cout << " " <<  size << " " << _size;

		delete[] memblock;
	}
	system("pause");

}

