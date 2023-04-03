#include "common.hpp"

namespace slim
{

struct failure_desc
{
    int     id;
    string  desc;
};

struct failure
{
    int     failure_id;
    string  detail;
    string  code_position;
};

static failure_desc failure_desc_list[] =
{
    { 0x00, "Unknown Error"},
    { 0x01, "Invalid Parameter"},
    { 0x02, "Device Not Found"},
    { 0x03, "Window Not Found"},
    { 0x04, "Element Not Found"},
};

}