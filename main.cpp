#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <functional>

using namespace std;
namespace fs = std::filesystem;

void limpiarPantalla()
{
    system("clear");
}

void esperarEnter() {
    std::cout << "Presiona Enter para continuar...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpia el búfer de entrada
    std::cin.get(); // Espera a que el usuario presione Enter
}

namespace Analizador
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    using ascii::space;
    using phoenix::ref;
    using qi::_1;
    using qi::double_;

    template <typename Iterador>
    bool sumar(Iterador inicio, Iterador fin, double &suma)
    {
        bool resultado = qi::phrase_parse(inicio, fin, (double_[ref(suma) = _1] >> *(',' >> double_[ref(suma) += _1])), space);

        if (inicio != fin)
            return false;
        return resultado;
    }

    void analizarExpresion(std::string expresion)
    {
        double suma = 0;

        limpiarPantalla();
        if (sumar(expresion.begin(), expresion.end(), suma))
        {
            cout << "-------------------------" << endl;
            cout << "La expresion es: " << expresion << endl;
            cout << "La suma de los numeros es: " << suma << endl;
            cout << "-------------------------" << endl;
        }
        else
        {
            cout << "-----------------------------------------" << endl;
            cout << "HUBO UN ERROR AL ANALIZAR LA EXPRESION" << endl;
            cout << "-----------------------------------------" << endl;
        }
        esperarEnter();
    }

}

namespace Menu
{
    class Menu
    {
    private:
        string titulo;
        /* crear un vector de opciones */
        vector<string> opciones;

        /* crear un vector de funciones */
        vector<std::function<void()>> funciones;

        bool salir = false;

    public:
        Menu(string titulo);
        ~Menu();
        void agregarOpcion(string opcion, std::function<void()> funcion);
        void mostrarMenu();
        void ejecutarOpcion(string opcion);
        void solicitarOpcion();
        void iniciarMenu();
        void opcionSalir();
        void opcionVolver();
    };

    Menu::Menu(string titulo)
    {
        this->titulo = titulo;
    }

    Menu::~Menu()
    {
    }

    void Menu::opcionSalir()
    {
        this->opciones.push_back("Salir");
        this->funciones.push_back([this]()
                                  { std::exit(0); });
    }

    void Menu::opcionVolver()
    {
        this->opciones.push_back("Volver");
        this->funciones.push_back([this]()
                                  { this->salir = true; });
    }

    void Menu::iniciarMenu()
    {
        while (!this->salir)
        {
            this->mostrarMenu();
            this->solicitarOpcion();
        }
    }

    void Menu::agregarOpcion(string opcion, std::function<void()> funcion)
    {
        this->opciones.push_back(opcion);
        this->funciones.push_back(funcion);
    }

    void Menu::mostrarMenu()
    {
        limpiarPantalla();
        cout << "================================================================" << endl;
        cout << "/" << this->titulo << endl;
        cout << "================================================================" << endl;
        for (int i = 0; i < this->opciones.size(); i++)
        {
            cout << i + 1 << ". " << this->opciones[i] << endl;
        }
    }

    void Menu::solicitarOpcion()
    {
        string opcion;
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        this->ejecutarOpcion(opcion);
    }

    void Menu::ejecutarOpcion(string opcion)
    {
        // validar que la opcion sea un numero
        if (opcion.find_first_not_of("0123456789") != string::npos)
        {
            cout << "X-> Debe ingresar un numero en el rango " <<
                1 << " a " << this->opciones.size()  << endl;
            esperarEnter();
            return;
        }

        // convertir la opcion a entero
        int opcionInt = std::stoi(opcion);

        if (opcionInt >= 1 && opcionInt <= this->opciones.size())
        {
            this->funciones[opcionInt - 1]();
        }
        else if (opcionInt == this->opciones.size() + 1)
        {
            this->salir = true;
        }
        else
        {
            cout << "X-> Opcion no valida" << endl;
        }
    }

}

namespace Funcionalidad
{

    namespace MenuAnalizarArchivo
    {

        void expresionDesdeArchivo(std::string rutaArchivo)
        {
            string contenido;
            ifstream archivo(rutaArchivo);

            if (!archivo)
            {
                cout << "-----------------------------------------" << endl;
                cerr << "No se pudo abrir el archivo" << endl;
                cout << "-----------------------------------------" << endl;
                esperarEnter();
                return;
            }

            ostringstream buffer;
            buffer << archivo.rdbuf(); // Lee todo el contenido del archivo en el buffer
            contenido = buffer.str(); // Convierte el buffer a un string
            Analizador::analizarExpresion(contenido);
        }

        void menuSeleccionarArchivo(std::string carpeta = "./")
        {
            Menu::Menu menuSeleccionarArchivoFile("Analizador Sintactico/ Analizar Archivo/ Seleccionar Archivo");

            try
            {
                for (const auto &entry : fs::directory_iterator(carpeta))
                {
                    if (entry.is_regular_file())
                    {
                        std::string rutaArchivo = entry.path().string();
                        std::string nombreArchivo = entry.path().filename().string();

                        // agregar opciones al menu con nombres únicos y acciones específicas
                        menuSeleccionarArchivoFile.agregarOpcion(nombreArchivo, [rutaArchivo]()
                                                                 { expresionDesdeArchivo(rutaArchivo); });
                    }
                }
                menuSeleccionarArchivoFile.opcionVolver();
                menuSeleccionarArchivoFile.opcionSalir();
                // Iniciar el menú después de agregar las opciones
                menuSeleccionarArchivoFile.iniciarMenu();
            }
            catch (const fs::filesystem_error &e)
            {
                std::cerr << "Error al acceder a la carpeta: " << e.what() << std::endl;
            }
        }

        void escribirRutaArchivo()
        {
            std::string rutaArchivo;
            std::cout << "Escriba la ruta del archivo: ";
            std::cin >> rutaArchivo;

            std::ifstream archivo(rutaArchivo);

            if (!archivo)
            {
                cout<< "-----------------------------------------" << endl;
                std::cerr << "No se pudo abrir el archivo" << std::endl;
                cout<< "-----------------------------------------" << endl;
                esperarEnter();
                return;
            }

            std::ostringstream buffer;
            buffer << archivo.rdbuf(); // Lee todo el contenido del archivo en el buffer

            std::string contenido = buffer.str(); // Convierte el buffer a un string
            Analizador::analizarExpresion(contenido);
            archivo.close();
        }
    }

    namespace MenuPrincipal
    {
        void ingresarExpresion()
        {
            limpiarPantalla();
            cout << "================================================================" << endl;
            cout << "/Analizador Sintactico/Ingresar Expresion" << endl;
            cout << "================================================================" << endl;

            std::string expresion;
            std::cout << "Ingrese la expresion a analizar: ";
            std::cin >> expresion;

            Analizador::analizarExpresion(expresion);
        }

        void acercaDe()
        {
            limpiarPantalla();
            cout << "================================================================" << endl;
            cout << "/Analizador Sintactico/Acerca de" << endl;
            cout << "================================================================" << endl;
            std::cout << "Analizador Sintactico" << std::endl;
            std::cout << "Desarrollado por Equipo 1" << std::endl;
            std::cout << "para la materia de Diseño de Compiladores" << std::endl;
            std::cout << "Universidad Nacional Autonoma de Honduras" << std::endl;
            std::cout << "Campus Choluteca" << std::endl;
            cout << "============================================================================" << endl;
            esperarEnter();
        }

        void menuAnalizarArchivo()
        {
            Menu::Menu menuAnalizarArchivo("Analizador Sintactico /Analizar Archivo");

            menuAnalizarArchivo.agregarOpcion("Elegir archivo", []()
                                              { MenuAnalizarArchivo::menuSeleccionarArchivo("archivos/"); });
            menuAnalizarArchivo.agregarOpcion("Escribir ruta", []()
                                              { MenuAnalizarArchivo::escribirRutaArchivo(); });
            menuAnalizarArchivo.opcionVolver();
            menuAnalizarArchivo.opcionSalir();
            menuAnalizarArchivo.iniciarMenu();
        }

        Menu::Menu getMenuPrincipal()
        {
            Menu::Menu menuPrincipal("Analizador Sintactico");

            menuPrincipal.agregarOpcion("Analizar Archivo", []()
                                        { menuAnalizarArchivo(); });
            menuPrincipal.agregarOpcion("Ingresar expresion", []()
                                        { ingresarExpresion(); });
            menuPrincipal.agregarOpcion("Acerca de", []()
                                        { acercaDe(); });

            menuPrincipal.opcionSalir();
            return menuPrincipal;
        }
    }

}

int main()
{
    Menu::Menu menuPrincipal = Funcionalidad::MenuPrincipal::getMenuPrincipal();
    menuPrincipal.iniciarMenu();

    return 0;
}
