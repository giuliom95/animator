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

const QPointF	Utils::normalize(const QPointF v) {
	const auto dot = QPointF::dotProduct(v, v);
	const auto invlen = 1 / sqrt(dot);
	return invlen * v;
}