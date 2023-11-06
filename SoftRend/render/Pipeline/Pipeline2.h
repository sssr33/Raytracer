#pragma once
#include "Pipeline2Traits.h"
#include "Registers\SemanticRegisters.h"
#include "Registers\SemanticRegisterHelpers.h"

#include <type_traits>
#include <Helpers\Meta\ArgType.h>

template<typename TraitsT = Pipeline2Traits<>>
class Pipeline2 {
    using VSOperatorT = decltype(&TraitsT::VertexShaderType::operator());
    using PSOperatorT = decltype(&TraitsT::PixelShaderType::operator());
    using BlendOperatorT = decltype(&TraitsT::BlendType::operator());

public:
    using VSInputType = std::decay_t<typename ArgType<VSOperatorT>::template n_type<0>>;
    using VSOutputType = std::decay_t<typename ArgType<VSOperatorT>::return_type>;

    using PSInputType = std::decay_t<typename ArgType<PSOperatorT>::template n_type<0>>;
    using PSOutputType = std::decay_t<typename ArgType<PSOperatorT>::return_type>;

private:
};

#include "Pipeline2.hpp"
