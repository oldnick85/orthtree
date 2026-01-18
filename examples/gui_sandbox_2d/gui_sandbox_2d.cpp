/**
 * @file main.cpp
 * @brief GUI application for 2D spatial partitioning tree visualization
 * 
 * This application demonstrates a 2D orthtree structure for spatial partitioning.
 * Users can add/remove boxes, visualize tree structure, and inspect objects under cursor.
 * 
 * @version 1.0
 * @date 2024
 */

#include <gtkmm.h>
#include <cmath>
#include <format>
#include <iostream>
#include <print>
#include <sstream>
#include <unordered_map>

#include "./orthtree.h"

std::unordered_map<uint, Glib::ustring> mouse_buttons{
    {0, "No button"},
    {GDK_BUTTON_PRIMARY, "Primary"},
    {GDK_BUTTON_MIDDLE, "Middle"},
    {GDK_BUTTON_SECONDARY, "Secondary"},
};

using Tree_t   = orthtree::Tree<int, double, 2, 2>;
using Box_t    = Tree_t::Box_t;
using Point_t  = Box_t::Point_t;
using Vector_t = Point_t::Vector_t;

constexpr int timer_interval_ms                  = 100;
constexpr int window_width                       = 1000;  ///< Increased window width for right panel
constexpr int window_height                      = 630;
constexpr int window_grid_margin                 = 15;
constexpr double area_sz                         = 600.0;
constexpr double default_box_sz                  = 10.0;  ///< Default box size
constexpr double max_box_sz                      = 50.0;  ///< Max box size
constexpr double line_width                      = 2.0;
constexpr double clr_lvl                         = 1.0;
constexpr double clr_lvl_step                    = 0.2;
constexpr int lbl_help_size_request              = 30;
constexpr int lbl_help_margin_top                = 10;
constexpr int btn_clear_margin_top               = 5;
constexpr int box_size_label_margin_top          = 10;
constexpr int box_size_scale_margin_top          = 5;
constexpr int right_panel_size_request           = 350;
constexpr int label_margin_top                   = 10;
constexpr int label_margin_bottom                = 5;
constexpr int scrolled_window_min_content_height = 100;
constexpr int text_view_margin                   = 5;

/**
 * @class TreeArea
 * @brief Custom drawing area for orthtree visualization
 * 
 * Handles drawing of the spatial partitioning tree and mouse interactions.
 */
class TreeArea : public Gtk::DrawingArea
{
  public:
    /**
     * @brief Constructs a TreeArea
     */
    TreeArea()
    {
        set_content_width(static_cast<int>(area_sz));
        set_content_height(static_cast<int>(area_sz));

        set_draw_func(sigc::mem_fun(*this, &TreeArea::on_draw));
        m_area_click = Gtk::GestureClick::create();
        m_area_click->set_button(0);  // All mouse buttons
        m_area_click->signal_pressed().connect(sigc::mem_fun(*this, &TreeArea::on_window_mouse_pressed));
        add_controller(m_area_click);

        auto motion_controller = Gtk::EventControllerMotion::create();
        motion_controller->signal_motion().connect(sigc::mem_fun(*this, &TreeArea::on_mouse_motion));
        motion_controller->signal_leave().connect(sigc::mem_fun(*this, &TreeArea::on_mouse_leave));
        add_controller(motion_controller);
    }

    virtual ~TreeArea() {}

    /**
     * @brief Timer callback for periodic updates
     * @param timer_number Timer identifier
     * @return true to continue timer
     */
    bool on_timeout([[maybe_unused]] int timer_number)
    {
        queue_draw();
        return true;
    }

    /**
     * @brief Mouse motion handler
     * @param mouse_x X coordinate of mouse
     * @param mouse_y Y coordinate of mouse
     */
    void on_mouse_motion(double mouse_x, double mouse_y)
    {
        m_last_mouse_x = mouse_x;
        m_last_mouse_y = mouse_y;
        update_info_text();
    }

    /**
     * @brief Mouse leave handler
     */
    void on_mouse_leave()
    {
        m_last_mouse_x = -1;
        m_last_mouse_y = -1;
        update_info_text();
    }

    /**
     * @brief Mouse press handler
     * @param n_press Number of presses
     * @param mouse_x X coordinate
     * @param mouse_y Y coordinate
     */
    void on_window_mouse_pressed(int n_press, double mouse_x, double mouse_y)
    {
        const auto current_button = mouse_buttons[m_area_click->get_current_button()];
        std::println("Mouse pressed in area: button={}, press={}, x={}, y={}", current_button, n_press, mouse_x,
                     mouse_y);
        if (m_area_click->get_current_button() == GDK_BUTTON_PRIMARY)
        {
            add_box(mouse_x, mouse_y);
        }
        else if (m_area_click->get_current_button() == GDK_BUTTON_SECONDARY)
        {
            remove_box(mouse_x, mouse_y);
        }
        queue_draw();
        update_info_text();
    }

    /**
     * @brief Add a box at specified coordinates
     * @param c_x Center X coordinate
     * @param c_y Center Y coordinate
     */
    void add_box(double c_x, double c_y)
    {
        Box_t box{{c_x - m_box_sz, c_y - m_box_sz}, {c_x + m_box_sz, c_y + m_box_sz}};

        for (std::size_t i = 0; i < Tree_t::Dim(); ++i)
        {
            if ((box.PntMin()[i] < 0.0) or (box.PntMin()[i] > area_sz))
            {
                return;
            }

            if ((box.PntMax()[i] < 0.0) or (box.PntMax()[i] > area_sz))
            {
                return;
            }
        }
        ++m_id;
        m_tree.Add(m_id, box);
        update_info_text();
    }

    /**
     * @brief Remove box at specified coordinates
     * @param c_x X coordinate
     * @param c_y Y coordinate
     */
    void remove_box(double c_x, double c_y)
    {
        const auto inters = m_tree.FindIntersected(Box_t{{c_x, c_y}});
        for (const auto& inter : inters)
        {
            m_tree.Del(inter);
        }
        update_info_text();
    }

    /**
     * @brief Clear all boxes from tree
     */
    void Clear()
    {
        m_tree.Clear();
        m_id = 0;
        update_info_text();
    }

    /**
     * @brief Set box size for new boxes
     * @param size New box size
     */
    void set_box_sz(double size)
    {
        m_box_sz = std::clamp(size, 1.0, max_box_sz);
        queue_draw();
        update_info_text();
    }

    /**
     * @brief Get current box size
     * @return Current box size
     */
    double get_box_sz() const { return m_box_sz; }

    /**
     * @brief Update text buffers with current tree information
     */
    void update_info_text()
    {
        if (!m_objects_buffer || !m_sections_buffer || !m_tree_buffer)
            return;

        // Clear buffers
        m_objects_buffer->set_text("");
        m_sections_buffer->set_text("");
        m_tree_buffer->set_text("");

        // Get objects under cursor
        if (m_last_mouse_x >= 0 && m_last_mouse_y >= 0)
        {
            const auto inters = m_tree.FindIntersected(Box_t{{m_last_mouse_x, m_last_mouse_y}});
            std::stringstream sstr;
            sstr << "Cursor at: (" << m_last_mouse_x << ", " << m_last_mouse_y << ")\n";
            sstr << "Objects under cursor (" << inters.size() << "):\n";
            for (const auto& inter : inters)
            {
                sstr << "  ID: " << inter << "\n";
            }
            m_objects_buffer->set_text(sstr.str());
        }
        else
        {
            m_objects_buffer->set_text("Cursor outside drawing area");
        }

        // Get sections under cursor
        if (m_last_mouse_x >= 0 && m_last_mouse_y >= 0)
        {
            std::stringstream sstr;
            sstr << "Sections under cursor:\n";

            // Lambda to collect sections containing the point
            std::function<void(const Box_t& area, uint lvl)> collect_sections = [&](const Box_t& area, uint lvl) {
                if (area.PntMin()[0] <= m_last_mouse_x && m_last_mouse_x <= area.PntMax()[0] &&
                    area.PntMin()[1] <= m_last_mouse_y && m_last_mouse_y <= area.PntMax()[1])
                {
                    sstr << std::string(static_cast<std::size_t>(lvl * 2), ' ') << "Level " << lvl << ": " << area.Str()
                         << "\n";
                }
            };

            // Lambda for values (not used for sections)
            std::function<void(const Box_t&, const int&, uint)> ignore_values = [](const Box_t&, const int&, uint) {};

            m_tree.TraverseDeep(collect_sections, ignore_values);
            m_sections_buffer->set_text(sstr.str());
        }
        else
        {
            m_sections_buffer->set_text("Cursor outside drawing area");
        }

        // Get full tree structure
        std::stringstream ss_tree;
        ss_tree << "Tree structure (total objects: " << m_id << ")\n";
        ss_tree << "Box size: " << m_box_sz << "\n";
        ss_tree << "Tree depth info:\n";

        std::function<void(const Box_t& area, uint lvl)> on_level_cb = [&](const Box_t& area, uint lvl) {
            ss_tree << std::string(static_cast<std::size_t>(lvl * 2), ' ') << "Level " << lvl << ": " << area.Str()
                    << "\n";
        };

        std::function<void(const Box_t& box, const int& val, uint lvl)> on_value_cb = [&](const Box_t& box,
                                                                                          const int& val, uint lvl) {
            ss_tree << std::string(static_cast<std::size_t>(lvl * 2), ' ') << "> Object " << val << ": " << box.Str()
                    << "\n";
        };

        m_tree.TraverseDeep(on_level_cb, on_value_cb);
        m_tree_buffer->set_text(ss_tree.str());
    }

    /**
     * @brief Set text buffers for information display
     * @param objects Buffer for objects info
     * @param sections Buffer for sections info
     * @param tree Buffer for tree info
     */
    void set_info_buffers(Glib::RefPtr<Gtk::TextBuffer> objects, Glib::RefPtr<Gtk::TextBuffer> sections,
                          Glib::RefPtr<Gtk::TextBuffer> tree)
    {
        m_objects_buffer  = std::move(objects);
        m_sections_buffer = std::move(sections);
        m_tree_buffer     = std::move(tree);
        update_info_text();
    }

  protected:
    Glib::RefPtr<Gtk::GestureClick> m_area_click;     ///< Mouse click gesture
    Glib::RefPtr<Gtk::TextBuffer> m_objects_buffer;   ///< Buffer for objects info
    Glib::RefPtr<Gtk::TextBuffer> m_sections_buffer;  ///< Buffer for sections info
    Glib::RefPtr<Gtk::TextBuffer> m_tree_buffer;      ///< Buffer for tree info

    /**
     * @brief Drawing callback
     * @param cr_cx Cairo context
     * @param width Drawing area width
     * @param height Drawing area height
     */
    void on_draw(const Cairo::RefPtr<Cairo::Context>& cr_cx, [[maybe_unused]] int width, [[maybe_unused]] int height)
    {
        std::println("{}", __FUNCTION__);

        std::function<void(const Box_t& area, uint lvl)> on_level_cb = [=](const Box_t& area, uint lvl) {
            std::println("{} Level {}: {}", std::string(static_cast<std::size_t>(lvl) * 2, ' '), lvl, area.Str());
            cr_cx->set_line_width(line_width);
            cr_cx->set_source_rgb(0.0, clr_lvl, 0.0);
            cr_cx->rectangle(area.PntMin()[0], area.PntMin()[1], area.PntMax()[0] - area.PntMin()[0],
                             area.PntMax()[1] - area.PntMin()[1]);
            cr_cx->stroke();
        };

        std::function<void(const Box_t& box, const int& val, uint lvl)> on_value_cb = [=](const Box_t& box,
                                                                                          const int& val, uint lvl) {
            std::println("{}>Value {}: {}", std::string(static_cast<std::size_t>(lvl) * 2, ' '), val, box.Str());
            cr_cx->set_line_width(line_width);
            cr_cx->set_source_rgb(clr_lvl * (lvl - 1) * clr_lvl_step, 0.0, clr_lvl * (1.0 - (lvl - 1) * clr_lvl_step));
            cr_cx->rectangle(box.PntMin()[0], box.PntMin()[1], box.PntMax()[0] - box.PntMin()[0],
                             box.PntMax()[1] - box.PntMin()[1]);
            cr_cx->stroke();
        };
        m_tree.TraverseDeep(on_level_cb, on_value_cb);

        cr_cx->stroke();
    }

  private:
    int m_id = 0;                                          ///< ID counter for boxes
    Tree_t m_tree{Box_t{{0.0, 0.0}, {area_sz, area_sz}}};  ///< Orthtree instance
    double m_box_sz       = default_box_sz;                ///< Current box size
    double m_last_mouse_x = -1;                            ///< Last mouse X position
    double m_last_mouse_y = -1;                            ///< Last mouse Y position
};

/**
 * @class MainWindow
 * @brief Main application window
 * 
 * Contains the drawing area, control buttons, and information displays.
 */
class MainWindow : public Gtk::Window
{
  public:
    /**
     * @brief Constructs the main window
     */
    MainWindow()
    {
        set_title("GUI SANDBOX 2D - OrthTree Visualization");
        set_default_size(window_width, window_height);
        set_resizable(true);

        // Create main horizontal box
        m_main_box.set_orientation(Gtk::Orientation::HORIZONTAL);
        m_main_box.set_margin(window_grid_margin);
        set_child(m_main_box);

        // Left panel - drawing area
        m_left_panel.set_orientation(Gtk::Orientation::VERTICAL);
        m_left_panel.set_margin_end(window_grid_margin);
        m_left_panel.append(m_area);

        // Add help label and clear button to left panel
        m_lbl_help.set_size_request(static_cast<int>(area_sz), lbl_help_size_request);
        m_lbl_help.set_text("Left click - add box | Right click - delete box");
        m_lbl_help.set_margin_top(lbl_help_margin_top);
        m_left_panel.append(m_lbl_help);

        m_btn_clear.set_label("Clear All");
        m_btn_clear.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clear_clicked));
        m_btn_clear.set_margin_top(btn_clear_margin_top);
        m_left_panel.append(m_btn_clear);

        // Add box size control
        m_box_size_label.set_text("Box Size:");
        m_box_size_label.set_margin_top(box_size_label_margin_top);
        m_left_panel.append(m_box_size_label);

        m_box_size_scale.set_range(1.0, max_box_sz);
        m_box_size_scale.set_value(default_box_sz);
        m_box_size_scale.set_digits(1);
        m_box_size_scale.set_draw_value(true);
        m_box_size_scale.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_box_size_changed));
        m_box_size_scale.set_margin_top(box_size_scale_margin_top);
        m_left_panel.append(m_box_size_scale);

        // Right panel - information displays
        m_right_panel.set_orientation(Gtk::Orientation::VERTICAL);
        m_right_panel.set_hexpand(true);
        m_right_panel.set_size_request(right_panel_size_request, -1);

        // Create scrolled text views for info displays
        setup_info_display(m_objects_view, "Objects under cursor:");
        setup_info_display(m_sections_view, "Sections under cursor:");
        setup_info_display(m_tree_view, "Tree structure:");

        // Add panels to main box
        m_main_box.append(m_left_panel);
        m_main_box.append(m_right_panel);

        // Set up text buffers
        m_area.set_info_buffers(m_objects_view.get_buffer(), m_sections_view.get_buffer(), m_tree_view.get_buffer());

        // Set up timer
        sigc::slot<bool()> my_slot = sigc::bind(sigc::mem_fun(*this, &MainWindow::on_timeout), 0);
        auto conn                  = Glib::signal_timeout().connect(my_slot, timer_interval_ms);
    }

  protected:
    /**
     * @brief Timer callback
     * @param timer_number Timer identifier
     * @return true to continue timer
     */
    bool on_timeout(int timer_number)
    {
        m_area.on_timeout(timer_number);
        return true;
    }

    /**
     * @brief Clear button click handler
     */
    void on_button_clear_clicked() { m_area.Clear(); }

    /**
     * @brief Box size scale change handler
     */
    void on_box_size_changed() { m_area.set_box_sz(m_box_size_scale.get_value()); }

    /**
     * @brief Set up an information display with label and scrolled text view
     * @param text_view Text view to set up
     * @param label_text Label text
     */
    void setup_info_display(Gtk::TextView& text_view, const Glib::ustring& label_text)
    {
        Gtk::Label* label = Gtk::make_managed<Gtk::Label>(label_text);
        label->set_xalign(0.0);
        label->set_margin_top(label_margin_top);
        label->set_margin_bottom(label_margin_bottom);
        m_right_panel.append(*label);

        // Create scrolled window for text view
        Gtk::ScrolledWindow* scrolled_window = Gtk::make_managed<Gtk::ScrolledWindow>();
        scrolled_window->set_vexpand(true);
        scrolled_window->set_hexpand(true);
        scrolled_window->set_min_content_height(scrolled_window_min_content_height);

        // Configure text view
        text_view.set_editable(false);
        text_view.set_wrap_mode(Gtk::WrapMode::WORD_CHAR);
        text_view.set_monospace(true);
        text_view.set_top_margin(text_view_margin);
        text_view.set_left_margin(text_view_margin);
        text_view.set_right_margin(text_view_margin);
        text_view.set_bottom_margin(text_view_margin);

        scrolled_window->set_child(text_view);
        m_right_panel.append(*scrolled_window);
    }

  private:
    Gtk::Box m_main_box{Gtk::Orientation::HORIZONTAL};   ///< Main horizontal container
    Gtk::Box m_left_panel{Gtk::Orientation::VERTICAL};   ///< Left panel with controls
    Gtk::Box m_right_panel{Gtk::Orientation::VERTICAL};  ///< Right panel with info

    TreeArea m_area;  ///< Drawing area for tree visualization

    Gtk::Label m_lbl_help;    ///< Help instructions label
    Gtk::Button m_btn_clear;  ///< Clear button

    Gtk::Label m_box_size_label;                                ///< Label for box size control
    Gtk::Scale m_box_size_scale{Gtk::Orientation::HORIZONTAL};  ///< Box size control

    Gtk::TextView m_objects_view;   ///< Text view for objects info
    Gtk::TextView m_sections_view;  ///< Text view for sections info
    Gtk::TextView m_tree_view;      ///< Text view for tree structure
};

/**
 * @brief Main application entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return Application exit code
 */
int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create("org.gtkmm.examples.orthtree");
    return app->make_window_and_run<MainWindow>(argc, argv);
}