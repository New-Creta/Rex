#pragma once

namespace rex
{
	struct ViewportCreationInfo
	{
		float top_left_x;
		float top_left_y;
		
		float width;
		float height;

		float min_depth;
		float max_depth;
	};

	class Viewport
	{
	public:
		Viewport();
		Viewport(const ViewportCreationInfo& info);
		virtual ~Viewport();

		float top_left_x() const;
		float top_left_y() const;
		float width() const;
		float height() const;
		float min_depth() const;
		float max_depth() const;
			   
	private:
		float m_top_left_x;
		float m_top_left_y;

		float m_width;
		float m_height;

		float m_min_depth;
		float m_max_depth;
	};
}