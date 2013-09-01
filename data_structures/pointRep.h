#ifndef __DYNAMICTEXTURES_POINTREP_H_
#define __DYNAMICTEXTURES_POINTREP_H_

#include "mat.h"
#include "vec.h"

namespace DynamicTextures 
{
	class PointRep
	{
	public:
		//Constructors
		PointRep()
		{
			point_.x = point_.y = point_.z = 0.0; 
			point_.w = 1.0;
			normal_.x = normal_.y = normal_.z = 0.0;
			texture_.x = texture_.y = 0.0;
			heightField_ = 0.0;
			perturbed_ = false;
			textureCoodsPopulated_ = false;
		}

		PointRep(Angel::vec4 _p,Angel::vec3 _n)
		{
			point_ = _p;
			normal_ = _n;
			texture_.x = texture_.y = 0.0;
			heightField_ = 0.0;
			perturbed_ = false;
			textureCoodsPopulated_ = false;
		}
		

	public:
		//Point Datastructure
		Angel::vec4 point_;
		Angel::vec3 normal_;
		//Associated Texture Maps
		Angel::vec2 texture_;
		//Heighfield information 
		double heightField_;
		//Other information 
		bool perturbed_;
		bool textureCoodsPopulated_;

	};
}


#endif 