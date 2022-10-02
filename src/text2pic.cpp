#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <unordered_map>
#include <string>

#include <text2pic.h>

namespace ddk
{
  struct Character
  {
    uchar *data;
    Size size;
    Size bearing;
  };

  std::unordered_map<char32_t, Character> Characters;

  Size sizeOfChar;
  std::u32string encodeChar;

  void LoadFont(const std::string &font, const std::u32string &text, int fontSize, float lineScale)
  {
    Free();
    Characters.clear();
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
      std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
      std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    int maxAdvance = 0;
    int maxBearingY = 0;
    encodeChar = text;
    for (char32_t c : text)
    {
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        continue;
      }
      
      if (maxAdvance < face->glyph->advance.x)
      {
        maxAdvance = face->glyph->advance.x;
      }

      if (maxBearingY < face->glyph->bitmap_top)
      {
        maxBearingY = face->glyph->bitmap_top;
      }

      uchar *data = new uchar[face->glyph->bitmap.width * face->glyph->bitmap.rows];
      memcpy(data, face->glyph->bitmap.buffer, face->glyph->bitmap.width * face->glyph->bitmap.rows);

      Character character = {
        data,
        Size(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        Size(face->glyph->bitmap_left, face->glyph->bitmap_top),
        // static_cast<uint>(face->glyph->advance.x),
      };
      Characters.insert(std::pair<char32_t, Character>(c, character));
    }

    maxAdvance >>= 6;

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    
    sizeOfChar = Size(maxAdvance, maxBearingY * lineScale);
  }

  std::u32string FrameToChar(const Mat& frame)
  {
    int channels = frame.channels();
		uchar *data = frame.data;

		std::u32string chars;
		int h = (frame.rows - 1) / sizeOfChar.height + 1;
		int w = (frame.cols - 1) / sizeOfChar.width + 1;
		chars.resize(w * h);

    uint encodeScale = 255 / encodeChar.length() + 1;

		for (int i = 0; i < h; ++i)
		{
			for (int j = 0; j < w; ++j)
			{
				uint value = 0u;
				for (int offsetX = 0; offsetX < sizeOfChar.height; ++ offsetX)
				{
          int x = i * sizeOfChar.height + offsetX;
						if (x >= frame.rows) break;
					for (int offsetY = 0; offsetY < sizeOfChar.width; ++offsetY)
					{
						int y = j * sizeOfChar.width + offsetY;
						if (y >= frame.cols) break;
						int offset = (x * frame.cols + y) * channels;
						for (int channel = 0; channel < channels; ++channel)
						{
							value += data[offset + channel];
						}
					}
				}
				value /= channels * sizeOfChar.width * sizeOfChar.height;

				value /= encodeScale;

				chars[i * w + j] = encodeChar[value];
			}
		}

    return chars;
  }

  Mat GenFrame(const Mat& f)
  {
    std::u32string text = FrameToChar(f);

    Size screen = Size(f.cols, f.rows);
    Mat frame(screen, CV_8UC3);
		int h = (frame.rows - 1) / sizeOfChar.height + 1;
		int w = (frame.cols - 1) / sizeOfChar.width + 1;
    uchar *data = frame.data;
    int channels = frame.channels();
    memset(data, 0, screen.width * screen.height * channels);
    for (int x = 0; x < h; ++x)
    {
      for (int y = 0; y < w; ++y)
      {
        char32_t c = text[x * w + y];
        Character ch = Characters.at(c);

        int baseX = (x + 1) * sizeOfChar.height - 1;
        int baseY = y * sizeOfChar.width;

        int top = baseX - ch.bearing.height;
        int left = baseY + ch.bearing.width;

        int right = left + ch.size.width;
        int bottom = baseX + (ch.size.height - ch.bearing.height);

        if (top < 0) top = 0;
        if (bottom >= screen.height) bottom = screen.height - 1;
        if (left < 0) left = 0;
        if (right >= screen.width) right = screen.width - 1;

        for (int width = left; width < right; ++width)
        {
          for (int height = top; height < bottom; ++height)
          {
            int dataIndex = (height * screen.width + width) * channels;
            int bufIndex = ((height - top) * ch.size.width + (width - left));
            for (int i = 0; i < channels; ++i)
              if (data[dataIndex + i] == 0)
                data[dataIndex + i] = ch.data[bufIndex];
          }
        }
      }
    }

    return frame;
  }

  void Free()
  {
    for (auto &&item : Characters)
    {
      delete[] item.second.data;
    }
  }
};
