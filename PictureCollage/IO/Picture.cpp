/*
* Picture.cpp
*
*  Created on: Apr 27, 2012
*      Author: lubo
*/

/* namespace bee */
#include "Picture.h"
#include <iostream>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
namespace bee {
	using namespace boost::filesystem;
	using namespace boost;
	using namespace std;

	Picture::Picture() {
		height=0;
		width=0;
		imgData=0;
		filePath="";
	}
	Picture::Picture(const Picture &pic){
		filePath=pic.filePath;
		width=pic.width;
		height=pic.height;
		salientArea=pic.salientArea;
		imgData=pic.imgData;
	}
	Picture::~Picture() {

	}

	Picture::Picture(const json_spirit::Object &obj) {
		Picture();
		for (json_spirit::Object::size_type i = 0; i != obj.size(); ++i) {
			const json_spirit::Pair& jpair = obj[i];
			const string& name = jpair.name_;
			const json_spirit::Value& value = jpair.value_;

			if (name == "filePath") {
				this->filePath = value.get_str();
			} else if (name == "height") {
				this->height = value.get_int();
			} else if (name == "width") {
				this->width = value.get_int();
			} else if (name == "salientArea") {
				json_spirit::Array json_salient_array = value.get_array();

				vector<pair<int, int> > salientArea;
				int x, y;
				for (uint j = 0; j < json_salient_array.size(); ++j) {
					const json_spirit::Object &object =
						json_salient_array[j].get_obj();
					for (uint k = 0; k < object.size(); ++k) {
						const json_spirit::Pair& pair = object[k];
						const string& name = pair.name_;
						const json_spirit::Value& value = pair.value_;
						if (name == "x") {
							x = value.get_int();
						} else if (name == "y") {
							y = value.get_int();
						}
					}
					salientArea.push_back(std::make_pair(x, y));
				}
				this->salientArea = salientArea;
			}
		}
	}
	json_spirit::Object Picture::TransToJsonObject() {
		json_spirit::Object json_obj;
		json_obj.push_back(json_spirit::Pair("filePath", this->filePath));

		json_spirit::Array salientArea;
		for (uint i = 0; i < this->salientArea.size(); ++i) {
			json_spirit::Object point;
			point.push_back(json_spirit::Pair("x", this->salientArea[i].first));
			point.push_back(json_spirit::Pair("y", this->salientArea[i].second));
			salientArea.push_back(point);
		}
		json_obj.push_back(json_spirit::Pair("salientArea", salientArea));
		json_obj.push_back(json_spirit::Pair("width", this->width));
		json_obj.push_back(json_spirit::Pair("height", this->height));
		return json_obj;
	}

	void Picture::LoadImageData(){
		IplImage* img=cvLoadImage(this->filePath.c_str(),1);
		if (img==NULL){
			return ;
		}
		this->height=img->height;
		this->width=img->width;
		this->imgData=new unsigned char[height*width*4];	//·ÖÅäimgDataµÄ¿Õ¼ä
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				CvScalar s=cvGet2D(img,height-1-i,j);
				this->imgData[(i*width+j)*4+0]=s.val[2];
				this->imgData[(i*width+j)*4+1]=s.val[1];
				this->imgData[(i*width+j)*4+2]=s.val[0];
				this->imgData[(i*width+j)*4+3]=255;
			}
		}
		cvReleaseImage(&img);
	}
}