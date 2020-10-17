#include "record.hpp"

Record::Record(RecordType record_type):type(record_type)
{
}

Record::Record(RecordType record_type, std::vector<glm::vec3> record_data):type(record_type), data(record_data)
{
}
