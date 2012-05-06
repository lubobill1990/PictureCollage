#ifndef BASIC_H
#define BASIC_H
#include <Box2D/Common/b2Math.h>
#ifndef INFINITY
#define INFINITY 1000000.0
#endif
#define EPSILON 0.001   // Define your own tolerance
#define FLOAT_EQ(x,v) (((v - EPSILON) < x) && (x <( v + EPSILON)))

#define Min(x,y) ((x)>(y)?(y):(x))
#define Max(x,y) ((x)<(y)?(y):(x))

typedef int int32;
typedef float float32;
typedef unsigned int uint;

inline bool IsFloatValid(float32 x)
{
	if (x != x)
	{
		// NaN.
		return false;
	}

	float32 infinity = std::numeric_limits<float32>::infinity();
	return -infinity < x && x < infinity;
}

namespace bee{

	class Transform:public b2Transform{
	public:
		Transform(b2Transform &tx){
			this->position=tx.position;
			this->R=tx.R;
		}
		Transform(){
			this->SetIdentity();
		}
	};
	class Color{
	public:
		Color(){}
		Color(int32 _r,int32 _g,int32 _b):r(_r),g(_g),b(_b){}
		int32 r,g,b;
	};
	class Point{
	public:
		Point() {}
		Point(b2Vec2 vec):x(vec.x),y(vec.y){}
		Point(const float *vec):x(vec[0]),y(vec[1]){}
		/// ��������
		Point(float32 x, float32 y) : x(x), y(y) {}
		/// ��Ϊ0����
		void SetZero() {
			x = 0.0f; 
			y = 0.0f; 
		}
		/// ��������
		void Set(float32 x_, float32 y_) {
			x = x_;
			y = y_;
		}

		/// ȡ��
		Point operator -() const {
			Point v;
			v.Set(-x, -y); return v; 
		}
		/// ���
		Point operator +(const Point &point) const{
			return Point(this->x+point.x,this->y+point.y);
		}

		/// �˷�
		Point operator *(float32 scalar){
			return Point(this->x*scalar,this->y*scalar);
		}
		/// ����
		Point operator /(float32 number){
			return Point(this->x/number,this->y/number);
		}
		/// ����������������ֵ
		float32 operator () (int32 i) const
		{
			return (&x)[i];
		}
		/// ��������д����������point(1)=1.2;
		float32& operator () (int32 i)
		{
			return (&x)[i];
		}
		/// �ӷ�
		void operator += (const Point& v)
		{
			x += v.x; y += v.y;
		}
		/// ����
		void operator -= (const Point& v)
		{
			x -= v.x; y -= v.y;
		}
		/// �˷�
		void operator *= (float32 a)
		{
			x *= a; y *= a;
		}
		/// ����
		void operator /= (float32 a){
			x/=a; y/=a;
		}
		/// �����ĳ���
		float32 Length() const
		{
			return sqrt(x * x + y * y);
		}
		/// �������ȵ�ƽ��
		float32 LengthSquared() const
		{
			return x * x + y * y;
		}
		/// ���������ı�ṹ���е�xyֵ��ʹ֮����һ����λ��������Ϊ1������
		float32 Normalize()
		{
			float32 length = Length();
			if (length < EPSILON)
			{
				return 0.0f;
			}
			float32 invLength = 1.0f / length;
			x *= invLength;
			y *= invLength;

			return length;
		}
		/// �Ƿ�Ϊ�Ϸ������������ж������float�Ƿ�Ϸ�
		bool IsValid() const
		{
			return IsFloatValid(x) && IsFloatValid(y);
		}
		operator b2Vec2(){
			return b2Vec2(this->x,this->y);
		}
		float32 x, y;
	};

	inline bool operator ==(const Point &point1,const Point &point2){
		return FLOAT_EQ(point1.x,point2.x)&&FLOAT_EQ(point1.y,point2.y);
	}
	inline bool operator !=(const Point &point1,const Point &point2){
		return !(point1==point2);
	}
	/// ���
	inline Point operator -(const Point &point1,const Point &point2){
		return Point(point1.x-point2.x,point1.y-point2.y);
	}
};
#endif

