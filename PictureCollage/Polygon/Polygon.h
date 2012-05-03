#ifndef POLYGON_H
#define POLYGON_H

#include <Polygon/Common.h>

#include <algorithm>

namespace bee{
	class PolygonList;
	class Point;
	class Polygon;

	typedef enum _Position{IN_POLYGON=1,ON_POLYGON=2,OUT_POLYGON=4,NOT_ON_LINE=8,ON_LINE_ENDPOINT=0x000f,ON_LINE_NOT_ENDPOINT=0x0010,ON_LINE=0x001f} Position;
	typedef std::vector<Point> PointVector;

	class LineSegment
	{
	public:
		LineSegment(void);
		LineSegment(const LineSegment &line);
		LineSegment(float ax,float ay,float bx, float by);
		LineSegment(const Point &a,const Point &b);
		~LineSegment(void){};
		Point start,end;
		const Point& GetStart() const;
		const Point& GetEnd() const;
		void SetStart(const Point &s);
		void SetEnd(const Point &e);
		///һ���Ƿ����߶���,�����˵�
		bool TestPointOn(const Point &point) const;
		///һ���Ƿ����߶����ڵ�ֱ����
		bool TestPointOnLine(const Point &point);
		/// ��һ���߶��Ƿ��ڱ��߶���
		bool TestLineSegmentOn(const LineSegment &line);
		///���������߶��Ƿ��ཻ,���ҿ��Է��ؽ���
		Position TestIntersect(const LineSegment &line,Point &interPoint=Point()) const;
		///��ȡ�߶�б��
		float32 GetSlope();
		///���������߶η���
		bool TestColinear(const LineSegment &line);
		Point GetMidPoint() const;
		//ʹ�߶������ķ�����ת
		void Reverse();
		bool operator ==(const LineSegment &line);
		bool operator !=(const LineSegment &line);
	private:
		float32 e_slope;
		bool e_SlopeChanged;
	};

	class Polygon
	{
	public:
		Polygon(){
			this->e_HasValidated=false;
			this->e_HasTrimColinearEdges=false;
		}
		Polygon(float *points,int count,Transform &tx){
			Polygon();
			for (int i=0;i<count;++i)
			{
				Point tmp_point(points[i*2],points[i*2+1]);
				int j=(i+1)>=count?0:i+1;
				Point tmp_point2(points[j*2],points[2*j+1]);
				Point curVertice=GetWorldPoint(tmp_point,tx);
				Point nextvertice=GetWorldPoint(tmp_point2,tx);
				LineSegment *line=new LineSegment(curVertice,nextvertice);
				this->AddEdge(*line);
				delete line;
			}
		}
		Polygon(b2Vec2 vertices[],int count,b2Transform tx){
			Point* vs=new Point[count];
			for (uint i=0;i<count;++i)
			{
				vs[i].x=vertices[i].x;
				vs[i].y=vertices[i].y;
			}
			for (int i=0;i<count;++i)
			{
				Point curVertice=GetWorldPoint(vs[i],tx);
				Point nextvertice=GetWorldPoint(vs[i+1>=count?0:i+1],tx);
				LineSegment *line=new LineSegment(Point(curVertice),Point(nextvertice));
				this->AddEdge(*line);
				delete line;
			}
			delete [] vs;
		}
		Polygon(Point vertices[],int count,Transform &tx){
			Polygon();
			for (int i=0;i<count;++i)
			{
				Point curVertice=GetWorldPoint(vertices[i],tx);
				Point nextvertice=GetWorldPoint(vertices[i+1>=count?0:i+1],tx);
				LineSegment *line=new LineSegment(Point(curVertice),Point(nextvertice));
				this->AddEdge(*line);
				delete line;
			}
		}

		~Polygon(){};
		void AddEdge(const LineSegment &line){
			this->e_HasValidated=false;
			this->e_HasTrimColinearEdges=false;
			this->e_Edges.push_back(LineSegment(line));
		};
		void DelEdge(const LineSegment &line){
			this->e_HasValidated=false;
			this->e_HasTrimColinearEdges=false;
			this->e_Edges.erase(
				std::remove(this->e_Edges.begin(),this->e_Edges.end(),line),
				this->e_Edges.end());
		};
		LineSegment &operator[](uint i){
			return const_cast<LineSegment &>( (*const_cast<const Polygon *>(this))[i]);
		};
		const LineSegment &operator[](uint i) const{
			return this->e_Edges[i];
		};
		uint Size() const{
			return this->e_Edges.size();
		}
		/// ����һ�������,�������εĶ�����������ö���εĶ���ͱ����ö��������һ������εĽ���
		Polygon GetPolygonAddInterPointAsVertex(const Polygon &polygon) const;
		/// �����������ǰ������ε�ÿ���߿��ܲ�����β���������ҳ���ʱ�����е�
		PolygonList Reconstruct();
		/// �жϵ��Ƿ��ڶ������
		Position IsPointIn(const Point &point) const;
		/// ��ȡ��������β��໥�����Ĳ���
		PolygonList GetNotOverlapedArea(const Polygon &polygon) const;
		/// ��ȡ����������໥�����Ĳ���
		PolygonList GetOverlapdArea(const Polygon &polygon) const;
		/// ������κ���һ������κϲ�
		PolygonList Merge(const Polygon &polygon) const;
		/// ������μ�ȥ���������
		PolygonList Segment(const Polygon &polygon) const;
		/// ������μ�ȥ��������
		PolygonList Segment(const PolygonList &polygons);
		/// flush��ʾ�Ƿ���֮��Ҫˢ����ʾ,Ĭ��Ϊ��;clear��ʾ�ڻ�ǰ�Ƿ��������,Ĭ��Ϊ��;colorΪ������ɫ,Ĭ��Ϊ��ɫ 
		void Draw(int flush=1,int clear=0,const Color& color=Color(1,1,1)) const;
		/// ���ĳ���ߵ���һ����
		LineSegment& GetNextEdge(uint index){
			return const_cast<LineSegment &>(const_cast<const Polygon *>(this)->GetNextEdge(index));
		}
		const LineSegment& GetNextEdge(uint index) const{
			index=index%this->e_Edges.size();
			if (index+1==this->e_Edges.size())
			{
				return this->e_Edges[0];
			}else{
				return this->e_Edges[index+1];
			}
		}
	private:
		std::vector<LineSegment> e_Edges;
		bool e_HasValidated;
		bool e_HasTrimColinearEdges;
		LineSegment& GetTailEdge()
		{
			return this->e_Edges[this->e_Edges.size()-1];
		}
		LineSegment& GetHeadEdge()
		{
			return this->e_Edges[0];
		}

		/// ������Σ�ʹ����κϷ���
		/// ȥ������Ϊ0�ı�
		void TrimZeroLengthEdges();
		/// �ϲ����ߵı�
		void MergeColinearEdges();
	};

	class PolygonList{
	public:
		PolygonList(){};
		~PolygonList(){};
		void AddPolygon(Polygon *polygon)
		{
			this->e_PolygonList.push_back(polygon);
		}
		void AddPolygon(const PolygonList & polygons)
		{
			for (uint i=0;i<polygons.Size();++i)
			{
				Polygon *tmp_poly=new Polygon(polygons[i]);
				this->e_PolygonList.push_back(tmp_poly);
			}
		}
		void DelPolygon(const Polygon *polygon){
			this->e_PolygonList.erase(std::remove(this->e_PolygonList.begin(),this->e_PolygonList.end(),polygon));
		}
		void Clear()
		{
			e_PolygonList.clear();
		}
		uint Size() const{
			return this->e_PolygonList.size();
		}
		Polygon & operator[](uint i){
			return const_cast<Polygon &>(const_cast<const PolygonList *>(this)->operator[](i));
		}
		const Polygon & operator[] (uint i) const{
			return *this->e_PolygonList[i];
		}
		PolygonList Merge();
		void Draw(int flush=1,int clear=0,Color color=Color(1,1,1)) const;
	private:
		std::vector<Polygon*> e_PolygonList;
	};
};
#endif