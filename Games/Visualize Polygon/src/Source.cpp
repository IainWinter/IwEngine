#include "iw/engine/EntryPoint.h"
#include "iw/graphics/Mesh.h"
#include "iw/engine/Components/Timer.h"
#include "iw/common/algos/MarchingCubes.h"
#include <array>

class Layer 
	: public iw::Layer 
{
public:
	iw::Mesh polygonMesh;
	std::vector<glm::vec2> pVerts;
	std::vector<unsigned> pIndex;
	iw::Timer timer;
	size_t lastCount = 0;

	iw::Mesh lineMesh;
	std::array<glm::vec2, 2> lVerts;
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

		iw::ref<iw::Material> mat = REF<iw::Material>(
			Asset->Load<iw::Shader>("shaders/texture2d.shader")
		);
		mat->SetWireframe(true);

		iw::MeshDescription desc;
		desc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(2));

		polygonMesh  = (new iw::MeshData(desc))->MakeInstance();
		lineMesh     = (new iw::MeshData(desc))->MakeInstance();
		cutRightMesh = (new iw::MeshData(desc))->MakeInstance();
		cutLeftMesh  = (new iw::MeshData(desc))->MakeInstance();
		
		polygonMesh .Material = mat->MakeInstance();
		lineMesh    .Material = mat->MakeInstance();
		cutLeftMesh .Material = mat->MakeInstance();
		cutRightMesh.Material = mat->MakeInstance();

		polygonMesh.Material->Set("color", iw::Color(1, 1, 1));
		lineMesh   .Material->Set("color", iw::Color(1, .25, .3));

		cutLeftMesh .Material->Set("color", iw::Color::From255(112, 70, 219));
		cutRightMesh.Material->Set("color", iw::Color::From255(217, 125, 67));
		//cutLeftMesh .Material->SetWireframe(false);
		//cutRightMesh.Material->SetWireframe(false);

		auto ldata = lineMesh.Data;

		ldata->SetBufferDataPtr(iw::bName::POSITION, 2, lVerts.data());
		ldata->SetTopology(iw::MeshTopology::LINES);

		unsigned lindex[2] { 0, 1 };
		ldata->SetIndexData(2, lindex);
		
		lVerts[1] = glm::vec2(0, 1);
		lVerts[0] = glm::vec2(0, 0);

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
				CutPolygon();
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

			if (count == 1) {
				pIndex = { 0 };
				polygonMesh.Data->SetTopology(iw::MeshTopology::POINTS);
			}

			else if (count == 2) {
				pIndex = { 0, 1 };
				polygonMesh.Data->SetTopology(iw::MeshTopology::LINES);
			}

			else {
				pIndex = iw::TriangulatePolygon(pVerts);
				polygonMesh.Data->SetTopology(iw::MeshTopology::TRIANGLES);
			}

			UpdateMesh(polygonMesh, pVerts, pIndex);
		}

		Renderer->BeginScene();
		Renderer->	DrawMesh(iw::Transform(), lineMesh);
		Renderer->	DrawMesh(iw::Transform(), cutLeftMesh);
		Renderer->	DrawMesh(iw::Transform(), cutRightMesh);
		Renderer->	DrawMesh(iw::Transform(), polygonMesh);
		Renderer->EndScene();
	}

	// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
// intersect the intersection point may be stored in the floats i_x and i_y.
	char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
		float p2_x, float p2_y, float p3_x, float p3_y, float* i_x, float* i_y)
	{
		float s1_x, s1_y, s2_x, s2_y;
		s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
		s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

		float s, t;
		s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
		t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

		//if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
		{
			// Collision detected
			if (i_x != NULL)
				*i_x = p0_x + (t * s1_x);
			if (i_y != NULL)
				*i_y = p0_y + (t * s1_y);
			return 1;
		}

		return 0; // No collision
	}

	void CutPolygon() {
		clVerts.clear();
		crVerts.clear();
		clIndex.clear();
		crIndex.clear();

		glm::vec2 line = lVerts[1] - lVerts[0];
		glm::vec2 tangent = glm::vec2(-line.y, line.x);

		float lineLength = glm::length(line);

		for (size_t i = 0; i < pIndex.size(); i += 3)
		{
			glm::vec2& a = GetPoint(i);
			glm::vec2& b = GetPoint(i + 1);
			glm::vec2& c = GetPoint(i + 2);

			bool aRight = OnRight(i);
			bool bRight = OnRight(i + 1);
			bool cRight = OnRight(i + 2);

			if (aRight && bRight && cRight)
			{
				AddPoint(crVerts, crIndex, a);
				AddPoint(crVerts, crIndex, b);
				AddPoint(crVerts, crIndex, c);
			}

			else if (!aRight && !bRight && !cRight)
			{
				AddPoint(clVerts, clIndex, a);
				AddPoint(clVerts, clIndex, b);
				AddPoint(clVerts, clIndex, c);
			}

			else 
			{
				unsigned singlePoint = 0;
				if (aRight == cRight) singlePoint = 1;
				if (aRight == bRight) singlePoint = 2;

				unsigned i0 = i +  singlePoint;
				unsigned i1 = i + (singlePoint + 1) % 3;
				unsigned i2 = i + (singlePoint + 2) % 3;

				glm::vec2 v0 = GetPoint(i0);
				glm::vec2 v1 = GetPoint(i1);
				glm::vec2 v2 = GetPoint(i2);

				glm::vec2 v01;
				glm::vec2 v02;
				get_line_intersection(v0.x, v0.y, v1.x, v1.y, lVerts[0].x, lVerts[0].y, lVerts[1].x, lVerts[1].y, &v01.x, &v01.y);
				get_line_intersection(v0.x, v0.y, v2.x, v2.y, lVerts[0].x, lVerts[0].y, lVerts[1].x, lVerts[1].y, &v02.x, &v02.y);

				std::vector<glm::vec2>& singleVerts =  OnRight(i0) ? crVerts : clVerts;
				std::vector<unsigned>&  singleIndex =  OnRight(i0) ? crIndex : clIndex;
				std::vector<glm::vec2>& otherVerts  = !OnRight(i0) ? crVerts : clVerts;
				std::vector<unsigned>&  otherIndex  = !OnRight(i0) ? crIndex : clIndex;


				AddPoint(singleVerts, singleIndex, v0);
				AddPoint(singleVerts, singleIndex, v01);
				AddPoint(singleVerts, singleIndex, v02);

				AddPoint(otherVerts, otherIndex, v1);
				AddPoint(otherVerts, otherIndex, v02);
				AddPoint(otherVerts, otherIndex, v01);

				AddPoint(otherVerts, otherIndex, v1);
				AddPoint(otherVerts, otherIndex, v2);
				AddPoint(otherVerts, otherIndex, v02);
			}
		}

		UpdateMesh(cutLeftMesh,  clVerts, clIndex);
		UpdateMesh(cutRightMesh, crVerts, crIndex);
	}

	inline bool OnRight(unsigned index) {
		glm::vec2 line = lVerts[1] - lVerts[0];
		glm::vec2 tangent = glm::vec2(-line.y, line.x);

		return glm::dot(pVerts[pIndex[index]] - lVerts[0], tangent) > 0;
	}

	inline glm::vec2 GetPoint(unsigned index) {
		return pVerts[pIndex[index]];
	}

	void AddPoint(
		std::vector<glm::vec2>& verts,
		std::vector<unsigned>& indices,
		glm::vec2 point)
	{
		auto itr = std::find(verts.begin(), verts.end(), point);

		if (itr == verts.end())
		{
			indices.push_back(verts.size());
			verts.push_back(point);
		}

		else {
			indices.push_back(std::distance(verts.begin(), itr));
		}
	}

	//void FixLastTri(
	//	std::vector<glm::vec2>& verts,
	//	std::vector<unsigned>& indices) 
	//{
	//	size_t i = indices.size() - 3;

	//	unsigned& ia = pIndex[i];
	//	unsigned& ib = pIndex[i + 1];
	//	unsigned& ic = pIndex[i + 2];

	//	glm::vec2& a = pVerts[ia];
	//	glm::vec2& b = pVerts[ib];
	//	glm::vec2& c = pVerts[ic];

	//	if (iw::IsClockwise(a, b, c)) { // swap clockwise triangle
	//		unsigned it = ib;
	//		ib = ic;
	//		ic = it;
	//	}
	//}

	void UpdateMesh(
		iw::Mesh& mesh, 
		std::vector<glm::vec2>& position, 
		std::vector<unsigned>& index) 
	{
		mesh.Data->SetBufferDataPtr(iw::bName::POSITION, position.size(), position.data());
		mesh.Data->SetIndexData(                         index   .size(), index   .data());
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
