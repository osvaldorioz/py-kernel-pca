#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <Eigen/Dense>
#include <pybind11/numpy.h>
//#include <torch/torch.h>
//#include <torch/script.h>
#include <vector>

namespace py = pybind11;

// Función de Kernel RBF (Radial Basis Function)
/*
torch::Tensor rbf_kernel(const torch::Tensor& X, double gamma) {
    auto sq_dist = torch::cdist(X, X).pow(2);
    return torch::exp(-gamma * sq_dist);
}*/

// Implementación de Kernel PCA
pybind11::array_t<double> kernel_pca(const pybind11::array_t<double>& data, int n_components, double gamma) {
    // Convertir pybind11::array_t a Eigen::MatrixXd
    Eigen::MatrixXd X = pybind11::cast<Eigen::MatrixXd>(data);

    // Calcular la matriz de kernel
    Eigen::MatrixXd K = Eigen::MatrixXd::Zero(X.rows(), X.rows());
    for (int i = 0; i < X.rows(); ++i) {
        for (int j = 0; j < X.rows(); ++j) {
            K(i, j) = std::exp(-gamma * (X.row(i) - X.row(j)).squaredNorm());
        }
    }

    // Centrar la matriz de kernel
    Eigen::MatrixXd ones = Eigen::MatrixXd::Ones(X.rows(), X.rows()) / X.rows();
    K = K - ones * K - K * ones + ones * K * ones;

    // Obtener los autovalores y autovectores
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig_solver(K);
    Eigen::VectorXd eigenvalues = eig_solver.eigenvalues();
    Eigen::MatrixXd eigenvectors = eig_solver.eigenvectors();

    // Seleccionar los n_components principales
    Eigen::MatrixXd principal_components = eigenvectors.rightCols(n_components);

    // Convertir Eigen::MatrixXd de vuelta a pybind11::array_t
    return pybind11::cast(principal_components);
}

// Exportar a Python
PYBIND11_MODULE(kpca_module, m) {
    m.def("kernel_pca", &kernel_pca, "Kernel PCA",
          py::arg("data"), py::arg("n_components") = 2, py::arg("gamma") = 0.1);
}
