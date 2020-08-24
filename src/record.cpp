#include "record.hpp"

/// not yet

Point::Point()
{
	position = glm::vec3(0.0f, 0.0f, 0.0f);
}

Point::Point(glm::vec3 point_position):position(point_position)
{
}

Record::Record(RecordType record_type):type(record_type)
{
}

Record::Record(RecordType record_type, std::vector<glm::vec3> record_data):type(record_type), data(record_data)
{
}
