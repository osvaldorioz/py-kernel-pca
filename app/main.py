from fastapi import FastAPI
from fastapi.responses import FileResponse
from pydantic import BaseModel
import numpy as np
from typing import List
import matplotlib
import matplotlib.pyplot as plt
import seaborn as sns
import kpca_module
import json

matplotlib.use('Agg')  # Usar backend no interactivo
app = FastAPI()

# Definir el modelo para el vector
class VectorF(BaseModel):
    vector: List[float]
    
@app.post("/kernel-pca")
def calculo(num_samples: int, num_features: int, num_components: int, gamma: float):
    output_file_1 = 'dispersion_kpca.png'
    output_file_2 = 'heatmap_kpca.png'

    # 🔹 Generar datos de prueba (x muestras, y variables)
    np.random.seed(42)
    n_samples = num_samples
    n_features = num_features
    data = np.random.rand(n_samples, n_features) * 10

    # 🔹 Aplicar Kernel PCA en C++
    n_components = num_components
    #gamma = 0.1
    reduced_data = kpca_module.kernel_pca(data, n_components, gamma)

    # 🔹 Gráfico de dispersión de los componentes principales
    plt.figure(figsize=(8, 6))
    plt.scatter(reduced_data[:, 0], reduced_data[:, 1], c='red', alpha=0.7)
    plt.title('Kernel PCA - Gráfico de Dispersión')
    plt.xlabel('Componente 1')
    plt.ylabel('Componente 2')
    plt.grid(True)
    #plt.show()
    plt.savefig(output_file_1)

    # 🔹 Heatmap de la Matriz del Kernel (si es factible)
    K = np.exp(-gamma * np.linalg.norm(data[:, None] - data, axis=2) ** 2)

    plt.figure(figsize=(8, 6))
    sns.heatmap(K, cmap="coolwarm", xticklabels=False, yticklabels=False)
    plt.title('Kernel Matrix Heatmap')
    #plt.show()

    plt.savefig(output_file_2)
    plt.close()
    
    j1 = {
        "Grafica de dispersion": output_file_1,
        "Grafica Heatmap": output_file_2
    }
    jj = json.dumps(str(j1))

    return jj

@app.get("/kernel-pca-graph")
def getGraph(output_file: str):
    return FileResponse(output_file, media_type="image/png", filename=output_file)