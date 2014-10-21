#ifndef __INSTRUCTION_H_
#define __INSTRUCTION_H_

typedef enum INSTRUCTION {
		INVDNONE, USERLEAV, USERJOIN, USERLOGN, 
		LONGSUCC, SERVPING, USERPONG, PARTCHAT,
		GAMECHAT, TMOUTRST, GAMEMAKE, GAMRLEAV,
		GAMRJOIN, GAMRSLST, GAMESTAT, GAMRKICK,
		GAMESHUT, GAMEBEGN, GAMEDATA, GAMCDATA,
		GAMRDROP, GAMRSRDY, LOGNSTAT, MOTDLINE
};
//TODO:This class should be change
//     let some fields as a private member
//     and make it more maintainable
class KInstruction {
private:
	unsigned char* mp_body;
	unsigned int m_body_len;
	unsigned short m_length;//m_username_len + m_body_len + 2 = m_length
	char* mp_user;//username
	unsigned int m_username_len;//not include 0x0 string end character
	unsigned short m_serial;
	INSTRUCTION m_id : 8;//Eight bit
	int inner_pointer;
public:
	KInstruction();
	KInstruction(const KInstruction&);
	~KInstruction();
	void print();
	int init_body(int);
	//Operations for body section
	char* getString(char*, int);
	unsigned int getInt();
	unsigned short getShort();	
	unsigned char getChar();
    unsigned char* getBody();
	int toData(unsigned char*, int);
	void setInt(unsigned int);
	void setShort(unsigned short);
	void setChar(unsigned char);
    void setBodyLen(int);
    //Reset pointer
    void resetIP();
	//Method for get sections
	int setBody(unsigned char*, int);
	int setUsername(char*, int);
	char* getUsername();
	void setLength(unsigned short);
	unsigned short getLength();
	void setSerial(unsigned short);
	unsigned short getSerial();
	INSTRUCTION getId();
	void setId(INSTRUCTION);
};

struct KPacket {
	unsigned char m_count;//Instruction count;
	KInstruction* mp_instructions;//Instruction array
};
#endif 

