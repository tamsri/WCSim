#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include "transform.hpp"

class Transmitter {
public:
	Transmitter(Transform);

private:
	float transmitter_power_output_;
	Transform transform_;
};
#endif
