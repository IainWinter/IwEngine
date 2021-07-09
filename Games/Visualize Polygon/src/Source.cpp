#include "iw/engine/EntryPoint.h"
#include "iw/graphics/Mesh.h"
#include "iw/engine/Components/Timer.h"
#include "iw/common/algos/polygon2.h"
#include <array>

class Layer
	: public iw::Layer 
{
//private:
//	struct CachedMesh {
//		iw::Mesh mesh;
//		std::vector<glm::vec2> verts;
//		std::vector<unsigned> index;
//		size_t hash;
//	};
//	std::unordered_map<size_t, CachedMesh> cache; // key is verts_hash
//
//	iw::ref<iw::Material> d2_color;
//
	iw::Mesh create_empty_mesh()
	{
		iw::ref<iw::Material> d2_color = REF<iw::Material>(
			Asset->Load<iw::Shader>("shaders/texture2d.shader")
		);
		d2_color->SetWireframe(true);

		iw::MeshDescription desc;
		desc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(2));

		iw::Mesh mesh = (new iw::MeshData(desc))->MakeInstance();
		mesh.Material = d2_color;

		return mesh;
	}

	void update_mesh(
		iw::Mesh& mesh,
		std::vector<glm::vec2> verts,
		std::vector<unsigned>& index = std::vector<unsigned>(),
		bool triangulate = true)
	{
		if (verts.size() == 1) {
			index = { 0 };
			mesh.Data->SetTopology(iw::MeshTopology::POINTS);
		}

		else if (verts.size() == 2) {
			index = { 0, 1 };
			mesh.Data->SetTopology(iw::MeshTopology::LINES);
		}

		else if (triangulate) {

			index = iw::TriangulatePolygon(verts);

			/*iw::polygon_crack cracks = iw::CrackPolygon(verts);

			verts = {};
			index = {};
				
			for (auto& [v, i] : cracks)
			for (unsigned idx : i)
			{
				iw::AddPointToPolygon(verts, index, v[idx]);
			}*/

			mesh.Data->SetTopology(iw::MeshTopology::TRIANGLES);
		}

		mesh.Data->SetBufferData(iw::bName::POSITION, verts.size(), verts.data());
		mesh.Data->SetIndexData(index.size(), index.data());
	}

	//size_t hash_polygon(
	//	const std::vector<glm::vec2>& verts,
	//	const std::vector<unsigned>& index = {})
	//{
	//	size_t hash = 0;
	//	for (const glm::vec2& vert : verts) {
	//		hash ^= (size_t)vert.x ^ 0x173537 * (size_t)vert.y;
	//	}

	//	for (unsigned i : index) {
	//		hash ^= 0x34613337 * (size_t)i;
	//	}

	//	return hash;
	//}

	//void poly(
	//	const std::vector<glm::vec2>& verts,
	//	const std::vector<unsigned>& index = {})
	//{
	//	if (verts.size()) return;

	//	//size_t hash = hash_polygon(verts, index);

	//	//auto itr = cache.find(hash);
	//	//if (itr == cache.end()) 
	//	//{
	//	//	CachedMesh& cached = cache[hash];
	//	//	cached.hash = hash;
	//	//	cached.mesh = create_empty_mesh();
	//	//	cached.verts = verts;
	//	//}

	//	//else
	//	//{
	//	//}

	//	iw::Mesh mesh = create_empty_mesh();

	//	update_mesh(mesh, verts, index);



	//}


	//void line(
	//	const glm::vec2& a,
	//	const glm::vec2& b)
	//{
	//	poly({ a, b });
	//}

public:

	

	iw::Mesh polygonMesh;
	std::vector<glm::vec2> pVerts;
	std::vector<unsigned> pIndex;
	iw::Timer timer;
	size_t lastCount = 0;

	iw::Mesh lineMesh;
	std::vector<glm::vec2> lVerts;
	bool lineDrag  = false;

	iw::Mesh cutLeftMesh;
	std::vector<glm::vec2> clVerts;
	std::vector<unsigned> clIndex;

	iw::Mesh cutRightMesh;
	std::vector<glm::vec2> crVerts;
	std::vector<unsigned> crIndex;

	glm::vec2 mposLast;

	Layer() 
		: iw::Layer("my layer")
	{}

	int Initialize() override
	{
		Renderer->SetClearColor(iw::Color::From255(33, 38, 41));

		iw::MeshDescription desc;
		desc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(2));

		polygonMesh  = create_empty_mesh();
		lineMesh     = create_empty_mesh();
		cutRightMesh = create_empty_mesh();
		cutLeftMesh  = create_empty_mesh();
		
		polygonMesh.Material->Set("color", iw::Color(1, 1, 1));
		lineMesh   .Material->Set("color", iw::Color(1, .25, .3));

		cutLeftMesh .Material->Set("color", iw::Color::From255(112, 70, 219));
		cutRightMesh.Material->Set("color", iw::Color::From255(217, 125, 67));

		lVerts.emplace_back(0.0f, 0.0f);
		lVerts.emplace_back(0.5f, 0.5f);

		update_mesh(lineMesh, lVerts);

		timer.SetTime("redraw", 0.1);

		return 0;
	}

	void PostUpdate() override {
		timer.Tick();

		glm::vec2 dim = glm::vec2(Renderer->Width(), -Renderer->Height());
		glm::vec2 mpos = iw::to_glm(iw::Mouse::ClientPos()) / dim;

		mpos = mpos * 2.0f - glm::vec2(1.0f, -1.0f);

		if (iw::Mouse::ButtonDown(iw::LMOUSE)) {

			if (   mpos.x < .9 && mpos.x > -.9f
				&& mpos.y < .9 && mpos.y > -.9f)
			{
				if (glm::distance(mpos, mposLast) > .05)
				{
					mposLast = mpos;
					pVerts.push_back(mpos);
				}
			}
		}

		/*if (iw::Keyboard::KeyDown(iw::P)) {
			std::vector<unsigned> index = iw::TriangulateSweep(pVerts);
			update_mesh(polygonMesh, pVerts, index, false);
		}*/

		if (iw::Keyboard::KeyDown(iw::SPACE)) {
			pVerts.clear();
			pIndex.clear();
		}

		if (iw::Mouse::ButtonDown(iw::RMOUSE)) {
			if (!lineDrag) {
				lVerts[0] = mpos;
				lineDrag = true;
			}

			else {
				lVerts[1] = mpos;
				auto [vl, il, vr, ir] = iw::CutPolygon(pVerts, pIndex, lVerts[0], lVerts[1]);
				clVerts = vl;
				clIndex = il;
				crVerts = vr;
				crIndex = ir;

				update_mesh(cutLeftMesh,  clVerts, clIndex, false);
				update_mesh(cutRightMesh, crVerts, crIndex, false);
				update_mesh(lineMesh, lVerts);
			}

			lineMesh.Data->Outdated = true;
		}

		else if (lineDrag) {
			lineDrag = false;
		}

		size_t count = pVerts.size();

		if (count != lastCount)
		{
			lastCount = count;
			update_mesh(polygonMesh, pVerts, pIndex);
		}

		Renderer->BeginScene();
		Renderer->	DrawMesh(iw::Transform(), lineMesh);
		Renderer->	DrawMesh(iw::Transform(), cutLeftMesh);
		Renderer->	DrawMesh(iw::Transform(), cutRightMesh);
		Renderer->	DrawMesh(iw::Transform(), polygonMesh);
		Renderer->EndScene();
	}
};

struct App : iw::Application 
{
	App() : iw::Application() {
		PushLayer<Layer>();
	}
};

iw::Application* CreateApplication(
	iw::InitOptions& options) 
{
	return new App();
}

// magic functions

// void init() {}
// void draw() {}

// point(x)
// line(xy)
// rect(xy)
// poly(v, i)

// typedef vec -> vector
// typedef map -> unordered_map
