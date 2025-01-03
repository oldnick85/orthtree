#include <gtkmm.h>
#include <cmath>
#include <iostream>

std::map<uint, Glib::ustring> mouse_buttons{
    {0, "No button"},
    {GDK_BUTTON_PRIMARY, "Primary"},
    {GDK_BUTTON_MIDDLE, "Middle"},
    {GDK_BUTTON_SECONDARY, "Secondary"},
};

#include "./orthtree.h"

using Tree_t   = orthtree::Tree<int, double, 2, 2>;
using Box_t    = Tree_t::Box_t;
using Point_t  = Box_t::Point_t;
using Vector_t = Point_t::Vector_t;

constexpr int timer_interval_ms  = 100;
constexpr int window_sz          = 430;
constexpr int window_grid_margin = 15;
constexpr double area_sz         = 400.0;
constexpr float box_sz           = 10.0;
constexpr double line_width      = 2.0;
constexpr double clr_lvl         = 1.0;
constexpr double clr_lvl_step    = 0.2;

class TreeArea : public Gtk::DrawingArea
{
  public:
    TreeArea()
    {
        set_content_width(area_sz);
        set_content_height(area_sz);

        set_draw_func(sigc::mem_fun(*this, &TreeArea::on_draw));
        m_area_click = Gtk::GestureClick::create();
        m_area_click->set_button(0);  // All mouse buttons
        m_area_click->signal_pressed().connect(sigc::mem_fun(*this, &TreeArea::on_window_mouse_pressed));
        add_controller(m_area_click);

        auto motion_controller = Gtk::EventControllerMotion::create();
        motion_controller->signal_motion().connect(sigc::mem_fun(*this, &TreeArea::on_mouse_motion));
        add_controller(motion_controller);
    }

    virtual ~TreeArea() {}

    bool on_timeout([[maybe_unused]] int timer_number)
    {
        //std::cout << "Timer: " << timer_number;
        queue_draw();
        return true;
    }

    void on_mouse_motion([[maybe_unused]] double mouse_x, [[maybe_unused]] double mouse_y)
    {
        //std::cout << "Mouse motion area: " << x << ", " << y << std::endl;
    }

    void on_window_mouse_pressed(int n_press, double mouse_x, double mouse_y)
    {
        const auto current_button = mouse_buttons[m_area_click->get_current_button()];
        std::cout << "Mouse pressed in area: " << current_button << ", " << n_press << ", " << mouse_x << ", "
                  << mouse_y;
        if (m_area_click->get_current_button() == GDK_BUTTON_PRIMARY)
        {
            ++m_id;
            m_tree.Add(m_id, Box_t{{mouse_x - box_sz, mouse_y - box_sz}, {mouse_x + box_sz, mouse_y + box_sz}});
            m_vel[m_id] = Vector_t({1.0, 1.0});
        }
        else if (m_area_click->get_current_button() == GDK_BUTTON_SECONDARY)
        {
            const auto inters = m_tree.FindIntersected(Box_t{{mouse_x, mouse_y}});
            for (const auto& inter : inters)
            {
                m_tree.Del(inter);
            }
        }
        queue_draw();
    }

    void Clear() { m_tree.Clear(); }

  protected:
    Glib::RefPtr<Gtk::GestureClick> m_area_click;

    void on_draw(const Cairo::RefPtr<Cairo::Context>& cr_cx, [[maybe_unused]] int width, [[maybe_unused]] int height)
    {
        printf("%s\n", __FUNCTION__);

        std::function<void(const Box_t& area, uint lvl)> on_level_cb = [=](const Box_t& area, uint lvl) {
            printf("%s Level %u: %s\n", std::string(static_cast<std::size_t>(lvl) * 2, ' ').c_str(), lvl,
                   area.Str().c_str());
            cr_cx->set_line_width(line_width);
            cr_cx->set_source_rgb(0.0, clr_lvl, 0.0);
            cr_cx->rectangle(area.PntMin()[0], area.PntMin()[1], area.PntMax()[0] - area.PntMin()[0],
                             area.PntMax()[1] - area.PntMin()[1]);
            cr_cx->stroke();
        };

        std::function<void(const Box_t& box, const int& val, uint lvl)> on_value_cb = [=](const Box_t& box,
                                                                                          const int& val, uint lvl) {
            printf("%s>Value %u: %s\n", std::string(static_cast<std::size_t>(lvl) * 2, ' ').c_str(), val,
                   box.Str().c_str());
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
    int m_id = 0;
    std::unordered_map<int, Vector_t> m_vel;
    Tree_t m_tree{Box_t{{0.0, 0.0}, {area_sz, area_sz}}};
};

class MainWindow : public Gtk::Window
{
  public:
    MainWindow()
    {
        set_title("GUI SANDBOX 2D");
        set_default_size(window_sz, window_sz);
        set_resizable(false);
        m_grid.set_margin(window_grid_margin);
        set_child(m_grid);
        m_grid.attach(m_area, 0, 0);
        m_lbl_help.set_size_request(box_sz, box_sz);
        m_lbl_help.set_text("right click - add box\nleft click - delete box");
        m_grid.attach(m_lbl_help, 0, 1);
        m_btn_clear.set_label("Clear");
        m_btn_clear.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clear_clicked));
        m_grid.attach(m_btn_clear, 1, 1);

        sigc::slot<bool()> my_slot = sigc::bind(sigc::mem_fun(*this, &MainWindow::on_timeout), 0);
        auto conn                  = Glib::signal_timeout().connect(my_slot, timer_interval_ms);
    }

  protected:
    bool on_timeout(int timer_number)
    {
        m_area.on_timeout(timer_number);
        return true;
    }

    void on_button_clear_clicked()
    {
        std::cout << "The Button was clicked.";
        m_area.Clear();
    }

    Gtk::Label m_lbl_help;
    Gtk::Button m_btn_clear;
    Gtk::Grid m_grid;

    TreeArea m_area;
};

int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create("org.gtkmm.examples.base");

    return app->make_window_and_run<MainWindow>(argc, argv);
}