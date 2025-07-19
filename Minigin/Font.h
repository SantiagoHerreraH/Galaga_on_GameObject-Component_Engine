#pragma once
#include <string>

struct _TTF_Font;
namespace dae
{
	/**
	 * Simple RAII wrapper for a _TTF_Font
	 */
	class FontData final
	{
	public:
		_TTF_Font* GetData() const;
		explicit FontData(const std::string& fullPath, unsigned int size);
		~FontData();

		FontData(const FontData &) = delete;
		FontData(FontData &&) = delete;
		FontData & operator= (const FontData &) = delete;
		FontData & operator= (const FontData &&) = delete;
	private:
		_TTF_Font* m_font;
	};
}
