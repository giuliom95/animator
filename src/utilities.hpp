#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>
#include <iomanip>
#include <QtGui>

namespace Utils {
	const int		mapQtMouseBtn	(const Qt::MouseButton btn);
	const float		length			(const QPointF v);
	const QPointF	normalize		(const QPointF v);

	inline const int cycle(const int idx, const int size) {
		if(idx < 0)
			return size + (idx % size);
		return idx % size;
	}


	class Pixel {
	public:
		GLubyte r, g, b, a;
	};

	class Frame : public std::vector<Pixel> {
	public:
		const int w, h;
		inline Frame(const int w, const int h, const Pixel fill) :
			std::vector<Pixel>(w*h, fill), w{w}, h{h} {}

		inline Pixel operator ()(const int x, const int y) {
			return (*this)[x*w + y];
		}

		inline void print() {
			const auto& f = *this;
			for(auto x = 0; x < h; ++x) {
				for(auto y = 0; y < w; ++y) {
					std::cout << std::setw(3) << (int)(f[x*w + y].r) << " ";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	};
	
}

#endif