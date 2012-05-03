#pragma once
#include <IO/PictureIO.h>
class BodyData
{
public:
	BodyData(int id){
		this->m_Id=id;
	}
	~BodyData();
	int m_Id;
	bee::Picture *m_Image;
protected:
private:
};
