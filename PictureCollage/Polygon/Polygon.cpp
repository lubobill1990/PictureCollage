#include "Polygon.h"
#include <list>
#include <algorithm>
namespace bee{

	Point f_start_point;
	bool f_Less(const Point &p1,const Point &p2){
		return (p1-f_start_point).LengthSquared()<(p2-f_start_point).LengthSquared();
	}

	void sort_point_vector(PointVector &p_v,const Point&compare_point){
		f_start_point=compare_point;
		std::sort(p_v.begin(),p_v.end(),f_Less);
	}
	//tested
	Position Polygon::IsPointIn( const Point &point ) const
	{
		int n=this->e_Edges.size();
		int count=0;
		LineSegment line;
		line.start=point;
		line.end.x=-INFINITY;
		line.end.y=point.y;

		for (int i=0;i<n;++i)
		{
			LineSegment edge=this->e_Edges[i];
			//������ڶ���εı���
			if(edge.TestPointOn(point)){
				return ON_POLYGON;
			}
			//���edgeƽ����x��,���ж���һ��edge
			if (FLOAT_EQ(edge.start.y,edge.end.y))
			{
				continue;
			}
			//�������εĶ����ڲ��������ϻ��߲������ߴ����˶���εı�,���1
			if (line.TestPointOn(edge.start))
			{
				//�����ظ�����õ�
				if (edge.start.y>edge.end.y)
				{
					++count;
				}
			}else if (line.TestPointOn(edge.end))
			{
				//�����ظ�����õ�
				if (edge.end.y>edge.start.y)
				{
					++count;
				}
			}else if (line.TestIntersect(edge)==ON_LINE_NOT_ENDPOINT)
			{
				++count;
			}
		}
		if (count%2==1)
		{
			return IN_POLYGON;
		} 
		else
		{
			return OUT_POLYGON;
		}
	}
	//tested
	PolygonList Polygon::Reconstruct()
	{
		this->TrimZeroLengthEdges();
		//��ŷ���ֵ
		PolygonList ret_val;
		//��������αߵ�ָ���list,����list��Ϊ��erase��Ч��
		std::list<LineSegment*> edge_list;
		for (uint i=0;i<this->Size();++i)
		{
			edge_list.push_back(&this->e_Edges[i]);
		}
		//��ǰ����Ķ����
		Polygon *cur_polygon=new Polygon();

		//����һ���б�,�����������,�������һ��һ���߶�û�м���,˵��������һϵ�б߲��������β�����Ķ����
		int operate_time=0;
		for(std::list<LineSegment*>::iterator itr=edge_list.begin();
			edge_list.size()>0;){
				//���itr������б�Ľ�β,�����¸�ֵitr���ײ�
				if (itr==edge_list.end())
				{
					if (operate_time==0)
					{
						//DEBUG_PRINT("��Ҫ������ϵıߵ��б�����λ����");
						throw Polygon(*this);
					}
					operate_time=0;
					itr=edge_list.begin();
					continue;
				}
				//���Ŀǰ����Ķ���λ�û��Ԫ��,��ѵ�ǰָ����ָ�ı�����
				if (cur_polygon->e_Edges.size()==0)
				{
					cur_polygon->AddEdge(**itr);
					itr=edge_list.erase(itr);
					continue;
				}

				/// �����ǰitr����ߵĿ�ʼ����ڵ�ǰ�����������һ���ߵĽ�β�ߵĽ�����,��:
				/// 1.�ѵ�ǰitr����ʾ�ı߲��뵱ǰ��������
				/// 2.��������б���ɾ����itr
				/// 3.���¸�ֵitr
				Point a=(*itr)->GetStart(),b=cur_polygon->GetTailEdge().GetEnd();
				if ((*itr)->GetStart()==cur_polygon->GetTailEdge().GetEnd())
				{
					cur_polygon->AddEdge(**itr);
					itr=edge_list.erase(itr);
					++operate_time;//��ʾ�й��ߵ����Ӳ���
				}else if ((*itr)->GetEnd()==cur_polygon->GetTailEdge().GetEnd())
				{
					LineSegment tmp_edge=**itr;
					tmp_edge.Reverse();
					cur_polygon->AddEdge(tmp_edge);
					itr=edge_list.erase(itr);
					++operate_time;
				}
				else{
					++itr;
				}
				//���ͷβ����,��˵����ǰ��������Ķ�����������,������뷵�ض�����б�
				if (cur_polygon->GetHeadEdge().GetStart()==cur_polygon->GetTailEdge().GetEnd())
				{
					ret_val.AddPolygon(cur_polygon);
					cur_polygon=new Polygon();
					continue;
				}
		}
		/// ������������õ��Ķ���Σ����ܻ������ڱ߹��ߵĿ��ܣ����úϲ��������ڱ������
		for (uint i=0;i<ret_val.Size();++i)
		{
			ret_val[i].MergeColinearEdges();
		}
		return ret_val;
	}

	PolygonList Polygon::GetNotOverlapedArea(const  Polygon &polygon ) const
	{
		PolygonList ret_val;
		Polygon poly1=this->GetPolygonAddInterPointAsVertex(polygon);
		Polygon poly2=polygon.GetPolygonAddInterPointAsVertex(*this);
		//������������û�н���
		if (poly1.Size()==this->Size()&&poly2.Size()==polygon.Size())
		{	
			if (this->IsPointIn(polygon[0].GetMidPoint())==IN_POLYGON)
			{
				//�������������ڱ�������ڣ��򱾶����ֻ����������εĲ��ֱ�����
				//����Ŀǰ��֧�ֿ��Ķ���Σ����Է��������������
				//TODO
				ret_val.AddPolygon(new Polygon(*this));
			}else if(polygon.IsPointIn((*this)[0].GetMidPoint())==IN_POLYGON)
			{
				//����������������������ڣ��򱾶����û�в������ǵĵط������Է���Ϊ��

			}else{
				//ʣ������Ϊ����������룬�򷵻ر������
				ret_val.AddPolygon(new Polygon(*this));
			}
			return ret_val;
		}
		//����н��㣬˵��������α���������ν�ȡ��һ��
		//��ȡ���������������������ⲿ�ıߣ�Ȼ��ȡ������������ڱ�������ڲ��ı�
		//���Ѱ���������Щ�ߵĶ���ε���Reconstruct���������ж����
		Polygon edges;
		for (uint i=0;i<poly1.Size();++i)
		{
			Position pos=polygon.IsPointIn(poly1[i].GetMidPoint());
			if (pos==OUT_POLYGON)
			{
				edges.AddEdge(poly1[i]);
			}
		}
		for (uint i=0;i<poly2.Size();++i)
		{
			Position pos=this->IsPointIn(poly2[i].GetMidPoint());
			if (pos==IN_POLYGON)
			{
				LineSegment tmp_edge=poly2[i];
				tmp_edge.Reverse();
				edges.AddEdge(tmp_edge);
			}
		}
		ret_val=edges.Reconstruct();
		return ret_val;
	}

	PolygonList Polygon::GetOverlapdArea(const Polygon &polygon ) const
	{
		PolygonList ret_val;
		Polygon poly1=this->GetPolygonAddInterPointAsVertex(polygon);
		Polygon poly2=polygon.GetPolygonAddInterPointAsVertex(*this);
		//������������û�н���,�򷵻������������ϳɵĶ�����б�
		if (poly1.Size()==this->Size()&&poly2.Size()==polygon.Size())
		{
			if (this->IsPointIn(polygon[0].GetMidPoint())==IN_POLYGON)
			{
				//��������Ķ�����ڱ�������ڣ��򷵻������Ķ����
				ret_val.AddPolygon(new Polygon(polygon));
			}else if (polygon.IsPointIn((*this)[0].GetMidPoint())==IN_POLYGON)
			{
				//����������������������ڣ��򷵻ر������
				ret_val.AddPolygon(new Polygon(*this));
			}else{
				//�������������໥û�й�ϵ���򷵻�0�����
			}
			return ret_val;
		}
		//����н���,���������ϲ��󷵻�
		Polygon edge_in_another;

		for (uint i=0;i<poly1.Size();++i)
		{
			Position pos=polygon.IsPointIn(poly1[i].GetMidPoint());

			if (pos==IN_POLYGON||/// poly1�ı���poly2�����棬˵����Ҫ�ϲ��ı�
				pos==ON_POLYGON)/// poly1�ı���poly2�ı��ϣ�˵������ͬ�ı�
			{
				edge_in_another.AddEdge(poly1[i]);
			}
		}

		for (uint i=0;i<poly2.Size();++i)
		{
			Position pos=this->IsPointIn(poly2[i].GetMidPoint());

			/// poly2�ıߵ��е���poly1�ϣ���ǰһ��ѭ����poly1�ߵ��е���poly2���ϵ������ͬ��ֻ��Ҫ��һ��
			if (pos==IN_POLYGON)/// poly2�ıߵ��е���poly1���棬˵����Ҫ�ϲ�������еı�
			{
				edge_in_another.AddEdge(poly2[i]);
			}
		}

		ret_val=edge_in_another.Reconstruct();
		return ret_val;
	}

	PolygonList Polygon::Merge(const Polygon &polygon) const
	{
		PolygonList ret_val;
		Polygon poly1=this->GetPolygonAddInterPointAsVertex(polygon);
		Polygon poly2=polygon.GetPolygonAddInterPointAsVertex(*this);
		//������������û�н���,�򷵻������������ϳɵĶ�����б�
		if (poly1.Size()==this->Size()&&poly2.Size()==poly2.Size())
		{
			if (this->IsPointIn(polygon[0].GetMidPoint())==IN_POLYGON)
			{
				//��������Ķ�����ڱ�������ڣ��򷵻ر������
				ret_val.AddPolygon(new Polygon(*this));
			}else if (polygon.IsPointIn((*this)[0].GetMidPoint())==IN_POLYGON)
			{
				//����������������������ڣ��򷵻����������
				ret_val.AddPolygon(new Polygon(polygon));
			}else{
				//�������������໥û�й�ϵ���򷵻����������
				Polygon *p1=new Polygon(*this);
				Polygon *p2=new Polygon(polygon);
				ret_val.AddPolygon(p1);
				ret_val.AddPolygon(p2);
			}
			return ret_val;
		}
		//����н���,���������ϲ��󷵻�
		Polygon edge_out_another;

		for (uint i=0;i<poly1.Size();++i)
		{
			Position pos=polygon.IsPointIn(poly1[i].GetMidPoint());

			if (pos==OUT_POLYGON||/// poly1�ı���poly2�����棬˵����Ҫ�ϲ��ı�
				pos==ON_POLYGON)/// poly1�ı���poly2�ı��ϣ�˵������ͬ�ı�
			{
				edge_out_another.AddEdge(poly1[i]);
			}
		}

		for (uint i=0;i<poly2.Size();++i)
		{
			Position pos=this->IsPointIn(poly2[i].GetMidPoint());

			/// poly2�ıߵ��е���poly1�ϣ���ǰһ��ѭ����poly1�ߵ��е���poly2���ϵ������ͬ��ֻ��Ҫ��һ��
			if (pos==OUT_POLYGON)/// poly2�ıߵ��е���poly1���棬˵����Ҫ�ϲ�������еı�
			{
				edge_out_another.AddEdge(poly2[i]);
			}
		}

		ret_val=edge_out_another.Reconstruct();

		return ret_val;
	}

	PolygonList Polygon::Segment(const Polygon &polygon ) const
	{
		PolygonList ret_val;
		ret_val=this->GetNotOverlapedArea(polygon);
		return ret_val;
	}

	PolygonList Polygon::Segment(const PolygonList &polygons )
	{
		PolygonList ret_val;
		if(polygons.Size()==0){
			ret_val.AddPolygon(this);
			return ret_val;
		}
		//��Ҫ���ָ�Ķ����
		PolygonList segmented_polygons;
		segmented_polygons.AddPolygon(this);

		for (uint i=0;i<polygons.Size();++i)
		{
			ret_val.Clear();
			const Polygon &segmenter=polygons[i];
			for(uint j=0;j<segmented_polygons.Size();++j)
			{
				Polygon &tmp_poly=segmented_polygons[j];
				PolygonList tmp_ret=tmp_poly.Segment(segmenter);
				ret_val.AddPolygon(tmp_ret);
			}
			segmented_polygons=ret_val;
		}
		return ret_val;
	}

	void Polygon::Draw(int flush/*=1*/,int clear/*=0*/,const Color& color/*=b2Color(1,1,1)*/) const
	{
		if (clear==1)
		{
			glClear (GL_COLOR_BUFFER_BIT);
		}
		glColor4f(color.r, color.g, color.b, 1.0f);

		// ��������Ϊ���ģʽ
		glPolygonMode(GL_FRONT, GL_FILL); 
		// ���÷���Ϊ����ģʽ
		glPolygonMode(GL_BACK, GL_LINE); 

		// ������ʱ�����һ�������
		glBegin(GL_LINE_LOOP);
		for (uint i=0;i<this->Size();++i)
		{
			glVertex2f((*this)[i].start.x,(*this)[i].start.y);
		}
		glEnd();

		glPointSize(4);//��Ĵ�С  
		glEnable(GL_POINT_SMOOTH);  

		glBegin(GL_POINTS);
		for (uint i=0;i<this->Size();++i)
		{
			glVertex2f((*this)[i].start.x,(*this)[i].start.y);
		}
		glEnd();
		if (flush)
		{
			glutSwapBuffers();
		}
	}
	/// ���ڱ�����ε�ÿһ����,�Ͳ�������ε�ÿһ���߽��в���
	/// ������β��Թ���,����һϵ�н���,�Խ����������,���ɱ�,���뵽���صĶ������
	Polygon Polygon::GetPolygonAddInterPointAsVertex( const Polygon &polygon ) const
	{
		Polygon ret_val;
		PointVector p_v;
		//���ڱ����Զ���ε�ÿһ����
		for (uint i=0;i<this->Size();++i)
		{
			LineSegment edge_a=(*this)[i];
			for (uint j=0;j<polygon.Size();++j)
			{
				LineSegment edge_b=polygon[j];
				// ������Ե��������ǹ���,����û�н���,��Ҫ���罻��
				if (!edge_a.TestColinear(edge_b))
				{
					Point inter_point;
					// ����н���,���ҽ��㲻�ڶ˵���
					if (edge_a.TestIntersect(edge_b,inter_point)==ON_LINE_NOT_ENDPOINT)
					{
						p_v.push_back(inter_point);
					}else if(edge_a.TestIntersect(edge_b,inter_point)==ON_LINE_ENDPOINT){

						if (inter_point!=edge_a.start&&inter_point!=edge_a.end)
						{
							p_v.push_back(inter_point);
						}
					}
				}
			}
			// ����н���,������,���ҹ����,����
			if (p_v.size()!=0)
			{
				//��p_v�ڵ�ֵ����,ʹ��ǰ����ĵ��뱻���Աߵ���ʼ��Խ��ԽԶ
				sort_point_vector(p_v,edge_a.GetStart());
				//�������Աߵ���ʼ���p_v�еĵ�һ���㹹��һ����,���Ҳ��뷵�صĶ����
				LineSegment tmp_edge=LineSegment(edge_a.GetStart(),p_v[0]);
				ret_val.AddEdge(tmp_edge);
				//�������Աߵ�ĩβ�����p_v��,ʹ����ӱߵĹ�����һ��forѭ�������
				p_v.push_back(edge_a.GetEnd());
				for (uint k=1;k<p_v.size();++k)
				{
					tmp_edge=LineSegment(p_v[k-1],p_v[k]);
					ret_val.AddEdge(tmp_edge);
				}
				//���
				p_v.clear();
			}else{
				// ���û�н���,��ֱ�Ӳ���ñ����Ա�
				ret_val.AddEdge(edge_a);
			}
		}
		ret_val.TrimZeroLengthEdges();
		return ret_val;
	}
	//tested
	void Polygon::TrimZeroLengthEdges()
	{
		if (this->e_HasValidated==true)
		{
			return;
		}
		/// 1.ȥ������Ϊ0�ı�(����ͷ�ͽ�β����ͬ�ı�)
		for (std::vector<LineSegment>::iterator itr=this->e_Edges.begin();
			itr!=e_Edges.end();)
		{
			//ȥ����ʼ�����ֹ����ͬ�ı�
			if (itr->start==itr->end)
			{
				itr=this->e_Edges.erase(itr);
			}else{
				++itr; //fix the bug. in the previous version, the iter increase even after one edge is erased from the polygon 
			}
		}		
		this->e_HasValidated=true;
	}
	//tested
	void Polygon::MergeColinearEdges()
	{
		if (this->e_HasTrimColinearEdges==true)
		{
			return;
		}
		/// �ϲ����ߵı�
		bool firstscan=true;
		for(std::vector<LineSegment>::iterator itr=this->e_Edges.begin();firstscan;)
		{
			std::vector<LineSegment>::iterator next_itr=itr+1;
			if(next_itr==this->e_Edges.end())
			{
				next_itr=this->e_Edges.begin();
				firstscan=false;
			}
			while(itr->TestColinear(*next_itr))
			{
				itr->end=next_itr->end;
				next_itr=this->e_Edges.erase(next_itr);
				if(next_itr==this->e_Edges.end()||!firstscan)
				{
					goto next_loop;
				}
			}
			++itr;
next_loop:
			;
		}
		this->e_HasTrimColinearEdges=true;
	}

	float32 ComputeB2VectorArea(std::vector<b2Vec2> vertices){
		if (vertices.size()<=2)
		{
			return 0;
		}
		b2Vec2 center;
		center.Set(0.0f, 0.0f);
		float32 area = 0.0f;
		float32 I = 0.0f;

		// pRef is the reference point for forming triangles.
		// It's location doesn't change the result (except for rounding error).
		b2Vec2 pRef(0.0f, 0.0f);
#if 0
		// This code would put the reference point inside the polygon.
		for (int32 i = 0; i < m_vertexCount; ++i)
		{
			pRef += m_vertices[i];
		}
		pRef *= 1.0f / count;
#endif

		const float32 k_inv3 = 1.0f / 3.0f;

		for (unsigned int i = 0; i < vertices.size(); ++i)
		{
			// Triangle vertices.
			b2Vec2 p1 = pRef;
			b2Vec2 p2 = vertices[i];
			b2Vec2 p3 = i + 1 < vertices.size() ? vertices[i+1] : vertices[0];

			b2Vec2 e1 = p2 - p1;
			b2Vec2 e2 = p3 - p1;

			float32 D = b2Cross(e1, e2);

			float32 triangleArea = 0.5f * D;
			area += triangleArea;

			// Area weighted centroid
			center += triangleArea * k_inv3 * (p1 + p2 + p3);

			float32 px = p1.x, py = p1.y;
			float32 ex1 = e1.x, ey1 = e1.y;
			float32 ex2 = e2.x, ey2 = e2.y;

			float32 intx2 = k_inv3 * (0.25f * (ex1*ex1 + ex2*ex1 + ex2*ex2) + (px*ex1 + px*ex2)) + 0.5f*px*px;
			float32 inty2 = k_inv3 * (0.25f * (ey1*ey1 + ey2*ey1 + ey2*ey2) + (py*ey1 + py*ey2)) + 0.5f*py*py;

			I += D * (intx2 + inty2);
		}

		// Total mass

		// Center of mass
		if (area<b2_epsilon)
		{
			area=0;
		}
		return area;
	}
	float32 Polygon::ComputeArea()
	{
		float32 retVal=0;
		PolygonList convexPolygons=TransPolygonToConvex(*this);
		for(uint i=0;i<convexPolygons.Size();++i){
			retVal+=ComputeB2VectorArea(TransToB2VecVector(convexPolygons[i]));
		}
		return retVal;
	}

	bool Polygon::TestConvex() const
	{
		LineSegment firstLine=this->operator[](this->Size()-1);
		LineSegment secondLine;
		for (uint i=0;i<this->Size();)
		{
			secondLine=this->operator[](i);
			b2Vec2 vector1=b2Vec2(firstLine.end.x-firstLine.start.x,firstLine.end.y-firstLine.start.y);
			b2Vec2 vector2=b2Vec2(secondLine.end.x-secondLine.start.x,secondLine.end.y-secondLine.start.y);
			if(b2Cross(vector1,vector2)<0){
				return false;
			}
			++i;
			firstLine=this->operator[](i-1);
		}
		return true;
	}

	Polygon Polygon::ConvexHull()
	{
		Polygon retVal=*this;
		for (uint i=0;i<retVal.Size();++i)
		{
			LineSegment &firstLine=retVal.GetEdgeIndexOf(i);
			LineSegment &secondLine=retVal.GetEdgeIndexOf(i+1);

			b2Vec2 vector1=b2Vec2(firstLine.end.x-firstLine.start.x,firstLine.end.y-firstLine.start.y);
			b2Vec2 vector2=b2Vec2(secondLine.end.x-secondLine.start.x,secondLine.end.y-secondLine.start.y);
			if(b2Cross(vector1,vector2)<=0){
				firstLine.end=secondLine.end;
				retVal.DelEdgeIndexOf(i+1);
				--i;
			}
		}

		for (uint i=0;i<retVal.Size();++i)
		{
			LineSegment &firstLine=retVal.GetEdgeIndexOf(i);
			LineSegment &secondLine=retVal.GetEdgeIndexOf(i+1);

			b2Vec2 vector1=b2Vec2(firstLine.end.x-firstLine.start.x,firstLine.end.y-firstLine.start.y);
			b2Vec2 vector2=b2Vec2(secondLine.end.x-secondLine.start.x,secondLine.end.y-secondLine.start.y);
			if(b2Cross(vector1,vector2)<=0){
				firstLine.end=secondLine.end;
				retVal.DelEdgeIndexOf(i+1);
				--i;
			}else{
				break;
			}
		}
		return retVal;
	}

	b2Polygon* TransBeePolygonTob2Polygon( const Polygon &input )
	{
		float32 *x=new float32[input.Size()];
		float32 *y=new float32[input.Size()];
		for (uint i=0;i<input.Size();++i)
		{
			x[i]=input[i].start.x;
			y[i]=input[i].start.y;
		}
		b2Polygon *output= new b2Polygon(x,y,input.Size());
		delete[]x;
		delete[]y;
		return output;
	}

	bee::PolygonList TransPolygonListToConvex( const PolygonList &input )
	{
		PolygonList ret_val;
		uint curIndex=0;
		for (uint i=0;i<input.Size();++i)
		{
			PolygonList tmpList=TransPolygonToConvex(input[i]);
			ret_val.AddPolygon(tmpList);
		}
		return ret_val;
	}

	bee::PolygonList TransPolygonToConvex( const Polygon &input )
	{
		PolygonList ret_val;
		if (input.TestConvex())
		{
			ret_val.AddPolygon(new Polygon(input));
		}else{
			b2Polygon *tmpPolygons=new b2Polygon[100];
			b2Polygon *poly_src=TransBeePolygonTob2Polygon(input);
			int32 polygon_count=DecomposeConvex(poly_src,tmpPolygons,100);
			for (uint i=0;i<polygon_count;++i)
			{
				ret_val.AddPolygon(new Polygon(&tmpPolygons[i]));
			}
			delete[] tmpPolygons;
			delete poly_src;
		}
		return ret_val;
	}

	std::vector<b2Vec2> TransToB2VecVector( const Polygon &polygon )
	{
		std::vector<b2Vec2> retVal;
		for (uint i=0;i<polygon.Size();++i)
		{
			retVal.push_back(b2Vec2(polygon[i].start.x,polygon[i].start.y));
		}
		return retVal;
	}


	float32 PolygonList::ComputeArea()
	{
		float32 retVal=0;
		for (uint i=0;i<this->Size();++i)
		{
			retVal+=this->e_PolygonList[i]->ComputeArea();
		}
		return retVal;
	}

	Polygon GetApproxiPolygonWithEdgeNumberLessThan( const Polygon &polygon, uint max_edge_number=8 )
	{
		Polygon retVal;
		double scale=double(polygon.Size())/max_edge_number;
		if (scale<=1)
		{
			retVal=Polygon(polygon);
		}else{
			scale=ceil(scale);
			uint overlapNum=polygon.Size()-8;
			Point *vertices=new Point[max_edge_number];
			uint edge_index=0;
			int count =1;
			for (uint i=0;i<polygon.Size();++i)
			{
				if (count>=scale)
				{
					vertices[edge_index]=(polygon[i].GetStart());
					++edge_index;
					count=0;
				}
				++count;
			}
			retVal=Polygon(vertices,edge_index);
			delete [] vertices;
		}
		return retVal;
	}

}
