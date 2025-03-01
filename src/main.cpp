#include "view.h"
#include "controller.h"

int main()
{
    View::iniciar();

    Controller controller;

    View::renderizar(controller);
    
    return 0;
}