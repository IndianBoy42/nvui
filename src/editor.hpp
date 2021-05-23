#ifndef NVUI_EDITOR_HPP
#define NVUI_EDITOR_HPP

#include <cstdint>
#include <msgpack.hpp>
#include <QFont>
#include <QObject>
#include <QRect>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QFontDatabase>
#include "hlstate.hpp"

// For easily changing the type of 'char' in a cell
using grid_char = QString;

struct GridChar
{
  std::uint16_t hl_id; // Shouldn't have more than 65k highlight attributes
  grid_char text;
  bool double_width = false;
};

struct Grid
{
  std::uint16_t x;
  std::uint16_t y;
  std::uint16_t rows;
  std::uint16_t cols;
  std::uint16_t id;
  std::vector<GridChar> area; // Size = rows * cols
  bool hidden = false;
};

/// Main editor area for Neovim
class EditorArea : public QWidget
{
  Q_OBJECT
public:
  EditorArea(
    QWidget* parent = nullptr,
    const HLState* state = nullptr
  );
  /**
   * Handles a Neovim "grid_resize" event.
   */
  void grid_resize(const msgpack::object* obj, std::uint32_t size);
  /**
   * Handles a Neovim "grid_line" event.
   */
  void grid_line(const msgpack::object* obj, std::uint32_t size);
  /**
   * Paints the grid cursor at the given grid, row, and column.
   */
  void grid_cursor_goto(const msgpack::object* obj, std::uint32_t size);
  /**
   * Handles a Neovim "option_set" event.
   */
  void option_set(const msgpack::object* obj, std::uint32_t size);
  /**
   * Handles a Neovim "flush" event.
   * This paints the internal buffer onto the window.
   */
  void flush();
private:
  QFontDatabase font_db;
  std::uint16_t charspace = 0;
  std::int16_t linespace = 0;
  const HLState* state;
  std::vector<Grid> grids;
  bool bold = false;
  // For font fallback, not used if a single font is set.
  std::vector<QFont> fonts;
  std::uint16_t font_width;
  std::uint16_t font_height;
  QFont font;
  /**
   * Sets the current font to new_font.
   */
  void set_guifont(const QString& new_font);
  /**
   * Adds text to the given grid number at the given row and col number,
   * overwriting the previous text a the position.
   */
  void set_text(
    Grid& g,
    grid_char c,
    std::uint16_t row,
    std::uint16_t col,
    std::uint16_t hl_id,
    std::uint16_t repeat = 1,
    bool is_dbl_width = false
  );
  /**
   * Returns a grid with the matching grid_num
   */
  Grid* find_grid(const std::uint16_t grid_num);
  /**
   * Converts a rectangle in terms of rows and cols
   * to a pixel-value rectangle relative to the top-left
   * corner of the editor area.
   * Also takes a grid number for the initial position.
   */
  QRect to_pixels(
    const std::uint16_t grid_num,
    const std::uint16_t start_row,
    const std::uint16_t start_col,
    const std::uint16_t end_row,
    const std::uint16_t end_col
  );
  /**
   * Updates the font metrics, such as font_width and font_height,
   * and sends a call to redraw the entire editor area.
   * This should be called after the font was changed.
   */
  void update_font_metrics();
protected:
};

#endif // NVUI_EDITOR_HPP
