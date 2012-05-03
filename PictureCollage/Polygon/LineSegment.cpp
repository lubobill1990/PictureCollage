#include "Polygon.h"
namespace bee{
	bool TestLineIntersect(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float &InterX, float &InterY)
	{ //�����߶��Ƿ��ཻX0X1 AND X1X2
		float x, y;
		float Minx01 = Min(x0, x1);
		float Miny01 = Min(y0, y1);
		float Minx23 = Min(x2, x3);
		float Miny23 = Min(y2, y3);
		float Maxx01 = Max(x0, x1);
		float Maxy01 = Max(y0, y1);
		float Maxx23 = Max(x2, x3);
		float Maxy23 = Max(y2, y3);

		if(x1!=x0 && x2!=x3)
		{
			float k1 = (y1-y0)/(x1-x0);
			float k2 = (y3-y2)/(x3-x2);
			float Den = (y1-y0)*(x3-x2) - (y3-y2)*(x1-x0);
			if(k1==k2)
			{ //ƽ�в��ཻ
				float d1 = abs(y0*(x1-x0)-x0*(y1-y0)-y2*(x3-x2)+x2*(y3-y2)); //���빫ʽd = abs(c1-c2) / sqrt(a*a+b*b)
				if(d1==0)
				{//ֱ���غ�
					if((x2>Minx01 && x2<Maxy01 && y2>Miny01 && y2<Maxy01) || (x3>Minx01 && x3<Maxy01 && y3>Miny01 && y3<Maxy01)
						|| (x0>Minx23 && x0<Maxy23 && y0>Miny23 && y0<Maxy23) || (x1>Minx23 && x1<Maxy23 && y1>Miny23 && y1<Maxy23))
					{  //ʵ����ײ�����߶��غ���Ϊ�ཻ��
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}   
			}
			x = ((y2-y0)*(x1-x0)*(x3-x2)+(y1-y0)*(x3-x2)*x0-(y3-y2)*(x1-x0)*x2)/Den;
			y = ((y1-y0)*(x-x0))/(x1-x0) + y0;

			if(Minx01<=x && x<=Maxx01 && Miny01<=y && y<=Maxy01 && Minx23<=x && x<=Maxx23 && Miny23<=y && y<=Maxy23)
			{
				InterX = x;
				InterY = y;
				return true;
			}
		}
		else if(x1==x0 && x2!=x3)
		{
			x = x0;
			y = ((y3-y2)*(x0-x2))/(x3-x2) + y2;
			if(Minx01<=x && x<=Maxx01 && Miny01<=y && y<=Maxy01 && Minx23<=x && x<=Maxx23 && Miny23<=y && y<=Maxy23)
			{
				InterX = x;
				InterY = y;
				return true;
			}
		}
		else if(x1!=x0 && x2==x3)
		{
			x = x2;
			y = ((y1-y0)*(x2-x0))/(x1-x0) + y0;
			if(Minx01<=x && x<=Maxx01 && Miny01<=y && y<=Maxy01 && Minx23<=x && x<=Maxx23 && Miny23<=y && y<=Maxy23)
			{
				InterX = x;
				InterY = y;
				return true;
			}       
		}
		return false;
	}
	LineSegment::LineSegment(void)
	{
		e_SlopeChanged=true;
	}

	LineSegment::LineSegment( const LineSegment &line )
	{
		LineSegment();
		this->start=line.GetStart();
		this->end=line.GetEnd();
	}

	LineSegment::LineSegment( float ax,float ay,float bx, float by )
	{
		LineSegment();
		this->start.x=ax;
		this->start.y=ay;
		this->end.x=bx;
		this->end.y=by;
	}

	LineSegment::LineSegment(const Point &a,const Point &b )
	{
		LineSegment();
		this->start=a;
		this->end=b;
	}

	bool LineSegment::TestPointOn( const Point &point ) const
	{
		Point a=point-this->start;
		Point b=this->end-point;
		//����ڶ˵�
		if ((a.x==0&&a.y==0)||(b.x==0&&b.y==0))
		{
			return true;
		}else if((b.x==0&&a.x!=0)||(b.y==0&&a.y!=0)){//Ϊ���������׼��,��ֹ������0
			return false;
		}else if ((b.x==0&&a.x==0&&b.y/a.y>0)||(b.y==0&&a.y==0&&b.x/a.x>0))//ˮƽ��ֱ�����
		{
			return true;
		}
		if ((a.x/b.x>0)&&FLOAT_EQ(a.x/b.x,a.y/b.y))
		{
			return true;
		}else {
			return false;
		}
	}

	Point LineSegment::GetMidPoint() const
	{
		return (this->start+this->end)/2;
	}

	Position LineSegment::TestIntersect( const LineSegment &p2,Point &inter_point ) const
	{
		//�����Ƿ񶥵���һ��
		if (this->TestPointOn(p2.GetStart()))
		{
			inter_point=p2.GetStart();
			return ON_LINE_ENDPOINT;
		}else if(this->TestPointOn(p2.GetEnd())){
			inter_point=p2.GetEnd();
			return ON_LINE_ENDPOINT;
		}else if(p2.TestPointOn(this->GetStart())){
			inter_point=this->GetStart();
			return ON_LINE_ENDPOINT;
		}else if(p2.TestPointOn(this->GetEnd())){
			inter_point=this->GetEnd();
			return ON_LINE_ENDPOINT;
		}
		bool res= TestLineIntersect(this->start.x,this->start.y,this->end.x,this->end.y,p2.start.x,p2.start.y,p2.end.x,p2.end.y,inter_point.x,inter_point.y);
		if (res&&(!(inter_point==p2.start||inter_point==p2.end)))
		{
			return ON_LINE_NOT_ENDPOINT;
		}else{
			return NOT_ON_LINE;
		}
	}

	bool LineSegment::TestLineSegmentOn( const LineSegment &line )
	{
		if (this->TestPointOn(line.start)&&this->TestPointOn(line.end))
		{
			return true;
		}else{
			return false;
		}
	}

	bool LineSegment::operator==( const LineSegment &line )
	{
		if (this->start==line.start&&this->end==line.end)
		{
			return true;
		}else{
			return false;
		}
	}

	bool LineSegment::operator!=( const LineSegment &line )
	{
		return !(this->operator==(line));
	}

	//����������㶼���߶����ڵ�ֱ���ϣ�����
	bool LineSegment::TestColinear( const LineSegment &line )
	{
		if (this->TestPointOnLine(line.start)&&this->TestPointOnLine(line.end))
		{
			return true;
		}else{
			return false;
		}
	}
	//���Ӹõ���߶ε�һ����Ϊ�߶Σ��Ѹ��߶κͱ�����߶αȽ�
	//���б����ͬ������ֱ����
	bool LineSegment::TestPointOnLine( const Point &point )
	{
		if (point==this->start||point==this->end)
		{
			return true;
		}
		LineSegment new_line=LineSegment(this->start,point);
		if (FLOAT_EQ(new_line.GetSlope(),this->GetSlope()))
		{
			return true;
		}else{
			return false;
		}
	}

	float32 LineSegment::GetSlope()
	{
		//����Ѿ������б�ʲ���û�иı�����㣬��ֱ�ӷ���ֵ
		if (this->e_SlopeChanged==false)
		{
			return this->e_slope;
		}
		//����߶������x���겻һ�������ó�������б��
		if (this->start.x!=this->end.x){
			this->e_slope=(this->start.y-this->end.y)/(this->start.x-this->end.x);
			if (this->e_slope>INFINITY)
			{
				this->e_slope=INFINITY;
			}
		}else{
			this->e_slope=INFINITY;
		}
		this->e_SlopeChanged=false;
		return this->e_slope;
	}

	void LineSegment::SetStart(const Point &s )
	{
		this->e_SlopeChanged=true;
		this->start=s;
	}

	void LineSegment::SetEnd( const Point &e )
	{
		this->e_SlopeChanged=true;
		this->end=e;
	}

	const Point &LineSegment::GetStart() const
	{
		return this->start;
	}

	const Point &LineSegment::GetEnd() const
	{
		return this->end;
	}
	void LineSegment::Reverse()
	{
		this->e_SlopeChanged=true;
		Point tmp=this->start;
		this->start=this->end;
		this->end=tmp;
	}
}
