#pragma once

namespace FormType
{
	inline constexpr std::array whitelist{
		RE::FormType::kFACT,
		RE::FormType::kCLAS,
		RE::FormType::kCSTY,
		RE::FormType::kRACE,
		RE::FormType::kOTFT,
		RE::FormType::kNPC_,
		RE::FormType::kVTYP,
		RE::FormType::kFLST,
		RE::FormType::kSPEL,
		RE::FormType::kARMO,
		RE::FormType::kLCTN,
		RE::FormType::kPERK
	};

	bool GetWhitelisted(RE::FormType a_type);
}
