#ifndef _MESSAGE_TYPE_H_
#define _MESSAGE_TYPE_H_
enum MessageType
{
	Login,
	Loginout,
	Delete,
	Add,
	Update,
	None,
	Exit
};

struct DataHead
{
	int dataLen=sizeof(DataHead);
	MessageType msType;
	char ms[128];
};
#endif // !_MESSAGE_TYPE_H_

