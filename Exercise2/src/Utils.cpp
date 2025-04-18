#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace PolygonalLibrary
{


// Funzione ImportMesh generale che chiama gli import di 0D, 1D e 2D e restituisce falso se non funzionano correttamente
bool ImportMesh(PolygonalMesh& mesh)
{

    if(!ImportCell0Ds(mesh))
        return false;

    if(!ImportCell1Ds(mesh))
        return false;

    if(!ImportCell2Ds(mesh))
        return false;

    return true;

}

// Import di Celle di dimensione 0 (punti)
bool ImportCell0Ds(PolygonalMesh& mesh)
{
    ifstream file("Cell0Ds.csv");

    if(file.fail())
        return false;

    // Creo una lista contenente le righe del documento .csv da leggere
    list<string> listLines;

    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    // rimuovo header
    listLines.pop_front();

    // Assegno la lunghezza della lista (ovvero il numero di punti) alla variabile
    // NumCell0Ds della PolygonalMesh mesh
    mesh.NumCell0Ds = listLines.size();

    if (mesh.NumCell0Ds == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    // Riservo il numero di celle necessarie nel vettore contentente i punti
    mesh.Cell0DsId.reserve(mesh.NumCell0Ds);
    // 3 legato alle 3 dimensioni
    mesh.Cell0DsCoordinates = Eigen::MatrixXd::Zero(3, mesh.NumCell0Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        char separator; // La variabile separator serve a contenere i separatori, in questo caso i ';'
        Vector2d coord;

        // L'oggetto di stream converter memorizza il primo carattere letto in id, in secondo in marker,
        // Il terzo nella matrice Cell0DsCoordinates alla riga 0, colonna id e così via
        converter >> id >> separator >> marker >> separator >> mesh.Cell0DsCoordinates(0, id) >> separator >> mesh.Cell0DsCoordinates(1, id);

        mesh.Cell0DsId.push_back(id);

        // Memorizza i marker in un dizionario del tipo chiave = marker, valore = lista vuota
        map<unsigned int, list<unsigned int>>& m = mesh.Cell0Ds_markers;
        // try_emplace(marker) prova a inserire nuova coppia chiave valore (dove il valore è una lista vuota)
        // se non esiste già, restituisce coppia di iteratore che punta alla nuova coppia e valore booleano
        // che è false se esiste già la coppia
        auto[itor, bool_val] = m.try_emplace(marker);
        // Prendi la coppia cui punta itor, aggiungi id al secondo elemento (ovvero il valore ovvero la lista)
        itor -> second.push_back(id);


    }

    return true;
}

// Import di celle di dimensione 1 (archi)

bool ImportCell1Ds(PolygonalMesh& mesh)
{
    ifstream file("Cell1Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    listLines.pop_front();

    mesh.NumCell1Ds = listLines.size();

    if (mesh.NumCell1Ds == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DsId.reserve(mesh.NumCell1Ds);
    // Memorizza gli estremi del segmento
    mesh.Cell1DsExtrema = Eigen::MatrixXi(2, mesh.NumCell1Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        char separator;
        Vector2i vertices;

        converter >> id >> separator >> marker >> separator >> mesh.Cell1DsExtrema(0, id) >> separator >> mesh.Cell1DsExtrema(1, id);
        mesh.Cell1DsId.push_back(id);

        map<unsigned int, list<unsigned int>>& m = mesh.Cell1Ds_markers;
        auto [itor, bool_val] = m.try_emplace(marker);
        itor->second.push_back(id);

    }

    return true;
}

// Import di celle di dimensione 2 (poligoni)

bool ImportCell2Ds(PolygonalMesh& mesh)
{
    ifstream file;
    file.open("Cell2Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    listLines.pop_front();

    mesh.NumCell2Ds = listLines.size();

    if (mesh.NumCell2Ds == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DsId.reserve(mesh.NumCell2Ds);
    // vectors dinamici contenenti i vertices e gli edges 
    mesh.Cell2DsVertices.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsEdges.reserve(mesh.NumCell2Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        unsigned int NumVertices;
        unsigned int NumEdges;
        char separator;
        double tmp;
        vector<unsigned int> vertices;
        vector<unsigned int> edges;

        converter >> id >> separator >> marker >> separator >> NumVertices;
        vertices.reserve(NumVertices);  // Il numero di vertici dipende dalla variabile letta dal file
        for(unsigned int i = 0; i < NumVertices; i++){  // Il ciclo itera in base al numero di vertici riportati nel .csv
            converter >> separator >> tmp;
            vertices.push_back(tmp);
        }

        converter >> separator >> NumEdges;
        edges.reserve(NumEdges);
        for(unsigned int i = 0; i < NumEdges; i++)
            {
            converter >> separator >> tmp;
            edges.push_back(tmp);}

        mesh.Cell2DsId.push_back(id);
        mesh.Cell2DsVertices.push_back(vertices);
        mesh.Cell2DsEdges.push_back(edges);

        map<unsigned int, list<unsigned int>>& m = mesh.Cell2Ds_markers;
        auto [itor, bool_val] = m.try_emplace(marker);
        itor->second.push_back(id);

    }

    return true;
}

}
