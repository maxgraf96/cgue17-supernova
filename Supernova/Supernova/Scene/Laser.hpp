#pragma once

#include "Model.hpp"

namespace supernova {
	namespace scene {
		class Laser : public Model
		{
		public:
			AABB boundingBox;

			Laser();
			Laser(glm::mat4& matrix, string const &path);
			~Laser();

			void update(float time_delta, int pressed) override {
				//not needed!
			}

			void setShooting(bool shooting);
			bool getShooting();

		private:
			bool shooting;
		};
	}
}
