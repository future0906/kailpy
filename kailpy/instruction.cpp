#include <string.h>
#include "instruction.h"
#include "common.h"
//Default construction didn't initialize body buffer
KInstruction::KInstruction()
{
	m_serial = 0;
	m_length = 2;
	m_id = INVDNONE;
	mp_user = NULL;
	mp_body = NULL;
	m_username_len = 0;
	m_body_len = 0;
	inner_pointer = 0;
}

//
int KInstruction::init_body(int body_length)
{
	if (mp_body != 0) {
		delete mp_body;
	}
	mp_body = new unsigned char[body_length];
	if (mp_body == 0) {
		__PRINT("Error:Not enough memory", NULL);
	}
	m_length = m_length - m_body_len + body_length;
	m_body_len = body_length;
    inner_pointer = 0;
    return 0;
}

KInstruction::KInstruction(const KInstruction& src_inst)
{
	int body_len = 0;
	m_serial = src_inst.m_serial;
	m_length = src_inst.m_length;
    m_body_len = src_inst.m_body_len;
	m_id = src_inst.m_id;
	//copy user name
	if (src_inst.mp_user == 0) {
		mp_user = 0;
		m_username_len = 0;
	}
	else {
		m_username_len = src_inst.m_username_len;
		mp_user = new char[m_username_len + 1];
		strncpy(mp_user, src_inst.mp_user, m_username_len + 1);
	}
	//copy body
	body_len = m_length - 2 - m_username_len;
	m_body_len = body_len; 
    inner_pointer = src_inst.inner_pointer;
	mp_body = new unsigned char[body_len];
	memcpy(mp_body, src_inst.mp_body, body_len);
}

KInstruction::~KInstruction()
{
	if (mp_user!= 0) {
		delete[] mp_user;
	}

	if (mp_body != 0) {
		delete[] mp_body;
	}
}
void KInstruction::print()
{

}
//if buf is not given(AKA NULL),this function will allocate
//one and return it
//so this function have two kind call
char* KInstruction::getString(char* buf, int len)
{
    char* result;
    if (buf == NULL) {
        result = new char[len];  
    }  
    else {
        result = buf;
    }
	strncpy(result, (char*)mp_body + inner_pointer, len);
	inner_pointer = inner_pointer + strlen(buf) + 1;
    return result;
}
unsigned short KInstruction::getShort()
{
	unsigned short temp;
	temp = *(unsigned short*)(mp_body + inner_pointer);
	inner_pointer += 2;
	return temp;
}
unsigned char KInstruction::getChar()
{
	unsigned char temp;
	temp = *(unsigned char*) (mp_body + inner_pointer);
	++inner_pointer;
	return temp;
}
int KInstruction::toData(unsigned char* temp, int len)
{
    unsigned char* result;
	unsigned int ptr = 0;

    if (len <= m_length + 2) {//not enough buffer
        return 0; 
    }

    if (temp == 0) {//no buffer
        __PRINT("[KInstruction::ToData]Buffer ptr error!\n", NULL);
        return 0;
    }
    else {//create buffer
        result = temp; 
    }

	*(unsigned short*)(result + ptr) = m_serial;
	ptr += 2;
	*(unsigned short*)(result + ptr) = m_length;
	ptr += 2;
	*(result + ptr) = m_id;
	++ptr;
	if (mp_user == 0) {//no user field
		*(result + ptr) = 0x0;
		++ptr;
	}
	else {
		strncpy((char*)result + ptr, mp_user, m_username_len + 1);
		ptr += m_username_len + 1;
	}
	//copy main body
	memcpy(result + ptr, mp_body, m_body_len);
    return 1;
}

void KInstruction::setInt(unsigned int value)
{
	*(unsigned int*)(mp_body + inner_pointer) = value;
	inner_pointer += 4;
}

void KInstruction::setShort(unsigned short value)
{
	*(unsigned short*)(mp_body + inner_pointer) = value;
	inner_pointer += 2;
}

void KInstruction::setChar(unsigned char value)
{
	*(unsigned char*)(mp_body + inner_pointer) = value;
	inner_pointer += 1;
}

int KInstruction::setBody(unsigned char* p_src, int len)
{
	if (mp_body != NULL) {//not empty
		delete mp_body;
	}
	mp_body = new unsigned char[len];
	if (!mp_body) {//memory not enough
		return -1;
	}
	memcpy(mp_body, p_src, len);
	//recalcuate length
	m_length = m_length - m_body_len + len;
	m_body_len = len;
	return 1;
}

void KInstruction::setBodyLen(int len)
{
    m_length = m_length - m_body_len + len;
    m_body_len = len;
}
unsigned char* KInstruction::getBody()
{
    return mp_body;
}
int KInstruction::setUsername(char* p_src, int len)
{
	if (mp_user != NULL) {//not empty
		delete mp_user;
	}
	mp_user = new char[len + 1];
	if (!mp_user) {//memory not enough
		return -1;
	}
	memcpy(mp_user, p_src, len);
	//recalcuate length
	m_length = m_length - m_username_len + len;
	m_username_len = len;
    mp_user[len] = '\0';
	return 1;
}

char* KInstruction::getUsername()
{
	return mp_user;
}

void KInstruction::setLength(unsigned short length)
{
	m_length = length;	
}

unsigned short KInstruction::getLength()
{
	return m_length;
}

void KInstruction::setSerial(unsigned short serial)
{
	m_serial = serial;	}

unsigned short KInstruction::getSerial()
{
	return m_serial;
}

void KInstruction::resetIP()
{
    inner_pointer = 0;
}
INSTRUCTION KInstruction::getId()
{
	return m_id;
}

void KInstruction::setId(INSTRUCTION id)
{
	m_id = id;	
}