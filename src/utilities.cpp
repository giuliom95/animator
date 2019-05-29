#include "utilities.hpp"

const int Utils::mapQtMouseBtn(const Qt::MouseButton btn) {
		switch(btn) {
		case Qt::LeftButton:
			return 0;
		case Qt::RightButton:
			return 1;
		case Qt::MiddleButton:
			return 2;
		default:
			return -1;
	}
}

const float		Utils::length(const QPointF v) {
	const auto dot = QPointF::dotProduct(v, v);
	return sqrt(dot);
}

const QPointF	Utils::normalize(const QPointF v) {
	const auto invlen = 1 / length(v);
	return invlen * v;
}
