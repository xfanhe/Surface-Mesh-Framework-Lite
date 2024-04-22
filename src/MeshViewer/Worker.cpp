#include"Worker.h"
#include"InteractiveViewerWidget.h"
#include<iostream>
#include<fstream>
#include<unordered_map>


typedef Eigen::Triplet<double> Tri; // row index, column index, value
typedef Eigen::SparseMatrix<double> SpMat;


void Worker::MinimalSurface(InteractiveViewerWidget* viewer, int iterate, double _lambda)
{
	std::vector<OpenMesh::Vec3d> vertices;
	Mesh& mesh = viewer->GetMesh();
	OpenMesh::Vec3d sum(0, 0, 0);
	for (int i = 0; i < iterate; ++i) {
		for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
			int count = 0;
			for (auto vv_it = mesh.vv_iter(*v_it); vv_it.is_valid(); ++vv_it) {
				sum += mesh.point(*vv_it);
				++count;
			}
			vertices.push_back(sum / count);
			sum = OpenMesh::Vec3d(0, 0, 0);
		}
		viewer->mutex.lock();
		for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
			if (mesh.is_boundary(*v_it)) {
				continue;
			}
			mesh.set_point(*v_it, mesh.point(*v_it) + _lambda * (vertices[v_it->idx()] - mesh.point(*v_it)));
		}
		viewer->mutex.unlock();
		vertices.clear();
		printf("Iteration %d lambda %f\n", i, _lambda);
		emit IterationFinished();
	}
	emit WorkFinished();
}

void Worker::Parameterization(InteractiveViewerWidget* viewer, int method)
{
	Mesh& mesh = viewer->GetMesh();

	// v_it->idx() : row index
	std::unordered_map<int, int> unknowns;
	std::unordered_map<int, int> boundary;

	///////////// Map from inner vertex idx to row num./////////////

	int n_boundary = 0;
	for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
		if (mesh.is_boundary(*v_it)) {
			++n_boundary;
		}
		else
			unknowns.insert({ (*v_it).idx(), static_cast<int>(unknowns.size()) });
	}

	///////////// Map from boundary vertex idx to the order of circle (theta)./////////////
	///////////// In the order of connectivity.////////////////////////

	OpenMesh::PolyConnectivity::VertexHandle v_start, v_next;
	for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
		if (mesh.is_boundary(*v_it)) {v_start = *v_it; break;}
	}

	v_next = v_start;
	do {
		boundary.insert({ v_next.idx(), static_cast<int>(boundary.size()) });

		for(auto heh : mesh.voh_range(v_next)) {
			if (mesh.is_boundary(mesh.to_vertex_handle(heh))) {
				v_next = mesh.to_vertex_handle(heh);
				break;
			}
		}

	} while (v_start != v_next);

	///////////// Construct the matrix A./////////////////////////////

	std::vector<Tri> coefficients;

	for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
		if (!mesh.is_boundary(*v_it)) {
			coefficients.push_back(Tri(unknowns.at(v_it->idx()), unknowns.at(v_it->idx()), v_it->valence()));
		}
		else {
			continue;
		}

		for (auto vv_it = mesh.vv_iter(*v_it); vv_it.is_valid(); ++vv_it) {
			if (mesh.is_boundary(*vv_it)) {
				continue;
			}
			coefficients.push_back(Tri(unknowns.at(v_it->idx()), unknowns.at(vv_it->idx()), -1));
		}
	}

	SpMat A(unknowns.size(), unknowns.size());
	A.setFromTriplets(coefficients.begin(), coefficients.end());
	Eigen::SimplicialCholesky<SpMat> chol(A);

	Eigen::MatrixXd out = A.toDense();
	std::ofstream("A.txt") << out;
	
	/////////////Construct the boundary vector b.////////////////////
	/////////////project the boundary onto unit circle.//////////////
	
	Eigen::VectorXd b_x(unknowns.size());
	Eigen::VectorXd b_y(unknowns.size());
	Eigen::VectorXd b_z(unknowns.size());
	b_x.setZero();
	b_y.setZero();
	b_z.setZero();

	for(auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
		if (!mesh.is_boundary(*v_it)) {
			for (auto vv_it = mesh.vv_iter(*v_it); vv_it.is_valid(); ++vv_it) {
				if (mesh.is_boundary(*vv_it)) {
					double theta = 2 * M_PI * boundary.at(vv_it->idx()) / n_boundary;
					b_x(unknowns.at(v_it->idx())) += cos(theta);
					b_y(unknowns.at(v_it->idx())) += sin(theta);
				}
			}
		}
	}

	std::vector<Eigen::VectorXd> b = { b_x, b_y, b_z };

	for (auto coordinate : { 0, 1, 2 }) {

		Eigen::VectorXd x = chol.solve(b[coordinate]);

		viewer->mutex.lock();
		for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
			if (mesh.is_boundary(*v_it)) {
				switch (coordinate)
				{
				case 0:
					mesh.point(*v_it)[coordinate] = cos(2 * M_PI * boundary.at(v_it->idx()) / n_boundary);
					break;
				case 1:
					mesh.point(*v_it)[coordinate] = sin(2 * M_PI * boundary.at(v_it->idx()) / n_boundary);
					break;
				case 2:
					mesh.point(*v_it)[coordinate] = 0;
					break;
				}
			}
			else
				mesh.point(*v_it)[coordinate] = x(unknowns.at(v_it->idx()));
		}
		viewer->mutex.unlock();
	}

	emit WorkFinished();
}
