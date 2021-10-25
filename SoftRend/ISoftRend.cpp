#include "ISoftRend.h"

class SoftRend : public ISoftRend
{
public:
private:
};

std::unique_ptr<ISoftRend> CreateISoftRend()
{
    return std::make_unique<SoftRend>();
}
