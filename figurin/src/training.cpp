#include <fann.h>
#include <iostream>

using namespace std;

int main(int argc, char**argv) {
    const unsigned int num_input = 3;
    const unsigned int num_output = 3;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 6;
    const float desired_error = (const float) 0.001;
    const unsigned int max_epochs = 1000000;
    const unsigned int epochs_between_reports = 1000;

    char * data = argv[1];
    char * data2 = argv[2];

    if ((data == NULL) || (data2 == NULL)) {
        cerr << "Debe especificar un archivo de datos y un archivo de salida como argumento." << endl;
        exit(1);
    }

    cout << "Usando datos de entrenamiento del archivo: " << data << endl;
    struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
    fann_train_on_file(ann, data, max_epochs, epochs_between_reports, desired_error);
    cout << "Guardando datos de la red al archivo: " << data2 << endl;
    fann_save(ann, data2);
    fann_destroy(ann);

    return 0;
}
