#include "RenderManager.h"

using namespace std;
using namespace RenderMath;

RenderManager * const RenderManager::GetInstance()
{
	static RenderManager instance;
	return &instance;
}
RenderManager::RenderManager()
{
	ResizeClient(800, 600);
	backgroundColor = RGB(190, 225, 235);
	SetCamera(1.0f, 4.0f/3.0f, 1.0f, 100.0f);

	SetVertexShader([](const Vertex * pInVertex, Vertex * pOutVertex) {
		memcpy(pOutVertex, pInVertex, sizeof(Vertex));
	}, sizeof(Vertex));

	SetPixelShader([](const PixelData * pInPixel, Color * pOutPixel) {
		*pOutPixel = pInPixel->color;
	});
}
RenderManager::~RenderManager()
{ }

void RenderManager::ResizeClient(const int width, const int height)
{
	this->width = width;
	this->height = height;
	depthBuffer.resize(height, width);
	screenBuffer.resize(height, width);
}

void RenderManager::SetCamera(const double inZoomX, 
							  const double inRadio, 
							  const double inNear, 
							  const double inFar)
{
	zoomX = inZoomX;
	zoomY = zoomX * inRadio;
	nearDis = inNear;
	farDis = inFar;

	double A = (farDis + nearDis) / (farDis - nearDis);
	double B = -2.0f * nearDis * farDis / (farDis - nearDis);
	clipMatrix = Matrix4X4(
		zoomX, 0.0f, 0.0f, 0.0f,
		0.0f, zoomY, 0.0f, 0.0f,
		0.0f, 0.0f, A, 1.0f,
		0.0f, 0.0f, B, 0.0f
		);
	clipPlanes[PD_UP].d = clipPlanes[PD_DOWN].d = clipPlanes[PD_RIGHT].d = clipPlanes[PD_LEFT].d = 0.0f;
	clipPlanes[PD_FAR].d = farDis; clipPlanes[PD_NEAR].d = nearDis;
	
	clipPlanes[PD_UP].normal.Normalize(0.0f, -zoomY, 1.0f, 0.0f);
	clipPlanes[PD_DOWN].normal.Normalize(0.0f, zoomY, 1.0f, 0.0f);
	clipPlanes[PD_LEFT].normal.Normalize(zoomX, 0.0f, 1.0f, 0.0f);
	clipPlanes[PD_RIGHT].normal.Normalize(-zoomX, 0.0f, 1.0f, 0.0f);
	clipPlanes[PD_FAR].normal.Normalize(0.0f, 0.0f, -1.0f, 0.0f);
	clipPlanes[PD_NEAR].normal.Normalize(0.0f, 0.0f, 1.0f, 0.0f);
}

void RenderManager::SetInVertexBuffer(const void * const pSource, const int vertexNum, size_t stride)
{
	ClearBuffer(inVertexBuffer);
	inVertexBuffer.resize(vertexNum, stride);
	memcpy(inVertexBuffer.data(), pSource, stride * vertexNum);
}

void RenderManager::SetIndexBuffer(const void * const pSource, const int indexNum)
{
	ClearBuffer(indexBuffer);
	indexBuffer.resize(indexNum);
	memcpy(indexBuffer.data(), pSource, sizeof(int) * indexNum);
}

void RenderManager::SetVertexShader(VertexShaderCallBack shaderCallBack, size_t outVertexStride)
{
	vertexShader = shaderCallBack;
	this->outVertexStride = outVertexStride;
}

void RenderManager::DoVertexShader()
{
	ClearBuffer(outVertexBuffer);
	int inVertexNum = inVertexBuffer.size();
	outVertexBuffer.resize(inVertexNum, outVertexStride);
	char * pInVertex = inVertexBuffer.data();
	char * pOutVertex = outVertexBuffer.data();
	size_t inVertexStride = inVertexBuffer.GetStride();
	size_t outVertexStride = outVertexBuffer.GetStride();

	for (int i = 0; i < inVertexNum; ++i) {
		vertexShader(reinterpret_cast<Vertex *>(pInVertex), 
					 reinterpret_cast<Vertex *>(pOutVertex));
		pInVertex += inVertexStride;
		pOutVertex += outVertexStride;
	}
}

void RenderManager::BeginRender()
{
	DoVertexShader();
	DoClipping();
	DoProjection();
	DoRasterization();
	DoPixelShader();
}


void RenderManager::DoClipping()
{
	ClearBuffer(clippedIndexBuffer);
	int indexNum = indexBuffer.size();
	int vertexNum = outVertexBuffer.size();
	// project vertex into clipping space
	for (int i = 0; i < vertexNum; i++) {
		outVertexBuffer[i]->position *= clipMatrix;
	}
	// do clipping
	IndexBuffer polyIndexBuffer;
	for (int i = 0; i < indexNum; i += 3) {
		// for each triangle, get index of three vertexes
		ClearBuffer(polyIndexBuffer);
		polyIndexBuffer.push_back(indexBuffer[i]);
		polyIndexBuffer.push_back(indexBuffer[i + 1]);
		polyIndexBuffer.push_back(indexBuffer[i + 2]);
		// clipping a triangle against six clip planes
		int oldVertexNum = outVertexBuffer.size();
		for (int i = PD_UP; i <= PD_NEAR; i++) {
			PlaneDir planeDir = static_cast<PlaneDir>(i);
			if (!ClipPolygon(&polyIndexBuffer, planeDir)) {
				outVertexBuffer.pop_back(outVertexBuffer.size() - oldVertexNum);
				break;
			}
		}
		// divide the output polygon into triangles if possible
		Triangulation(&polyIndexBuffer);
	}
}
int RenderManager::ClipPolygon(IndexBuffer * polyIndexBuffer, const PlaneDir planeDir)
{
	int indexNum = polyIndexBuffer->size();
	IndexBuffer outBuffer;
	// for (0. 1), (1, 2), ... , (N-1, 0) edge, do clipping
	for (int i = 0; i < indexNum - 1; ++i) {
		ClipEdge((*polyIndexBuffer)[i], 
				 (*polyIndexBuffer)[i + 1], 
				 planeDir, 
				 &outBuffer);
	}
	ClipEdge((*polyIndexBuffer)[indexNum - 1],
			 (*polyIndexBuffer)[0],
			 planeDir,
			 &outBuffer);
	*polyIndexBuffer = outBuffer;
	if (polyIndexBuffer->size() > 2) {
		return 1;
	} else {
		return 0;
	}
}
int RenderManager::ClipEdge(const int indexStart,
							const int indexEnd,
							const PlaneDir planeDir,
							IndexBuffer * outBuffer)
{
	Vertex * pStart = outVertexBuffer[indexStart];
	Vertex * pEnd = outVertexBuffer[indexEnd];
	double wStart = pStart->position.w + 0.00001f;
	double wEnd = pEnd->position.w + 0.00001f;
	double cStart = 0.0f;
	double cEnd = 0.0f;
	if (planeDir == PD_UP) {
		cStart = pStart->position.y;
		cEnd = pEnd->position.y;
	} else if (planeDir == PD_DOWN) {
		cStart = -pStart->position.y;
		cEnd = -pEnd->position.y;
	} else if (planeDir == PD_RIGHT) {
		cStart = pStart->position.x;
		cEnd = pEnd->position.x;
	} else if (planeDir == PD_LEFT) {
		cStart = -pStart->position.x;
		cEnd = -pEnd->position.x;
	} else if (planeDir == PD_FAR) {
		cStart = pStart->position.z;
		cEnd = pEnd->position.z;
	} else if (planeDir == PD_NEAR) {
		cStart = -pStart->position.z;
		cEnd = -pEnd->position.z;
	}

	if (cStart <= wStart) {
		if (cEnd <= wEnd) {
			// ==================== start in, end in  ====================
			// out put start
			outBuffer->push_back(indexStart);
			return 0;
		} else {
			// ==================== start in, end out ====================
			// output start
			outBuffer->push_back(indexStart);
			// output clip
			// insert the new vertex to vertex buffer and index buffer
			outVertexBuffer.push_back(outVertexBuffer[indexStart]);
			int indexClip = outVertexBuffer.size() - 1;
			outBuffer->push_back(indexClip);
			// set the value of new vertex by interpolation 
			double k = GetIntersectK(outVertexBuffer[indexStart]->position, 
									 outVertexBuffer[indexEnd]->position, 
									 planeDir);
			outVertexBuffer[indexClip]->setValueByLI(outVertexBuffer[indexEnd], k);
			return 1;
		}
	} else {
		if (cEnd <= wEnd) {
			// ==================== start out, end in  ====================
			// output clip
			// insert the new vertex to vertex buffer and index buffer
			outVertexBuffer.push_back(outVertexBuffer[indexStart]);
			int indexClip = outVertexBuffer.size() - 1;
			outBuffer->push_back(indexClip);
			// set the value of new vertex by interpolation 
			double k = GetIntersectK(outVertexBuffer[indexStart]->position,
									 outVertexBuffer[indexEnd]->position, 
									 planeDir);
			outVertexBuffer[indexClip]->setValueByLI(outVertexBuffer[indexEnd], k);
			return 1;
		} else {
			// ==================== start out, end out ====================
			// output no thing
			return 0;
		}
	}
}
double RenderManager::GetIntersectK(const Vector4 & posStart,
									const Vector4 & posEnd,
									const PlaneDir planeDir)
{
	Plane fixedClipPlane = clipPlanes[planeDir];
	fixedClipPlane.normal.x /= zoomX;
	fixedClipPlane.normal.y /= zoomY;
	fixedClipPlane.normal.z = 0.0f;
	fixedClipPlane.normal.w = clipPlanes[planeDir].normal.z;
	double disStart = abs(posStart.DotProduct(fixedClipPlane.normal) - fixedClipPlane.d);
	double disEnd = abs(posEnd.DotProduct(fixedClipPlane.normal) - fixedClipPlane.d);
	return disStart / (disStart + disEnd);
}

void RenderManager::Triangulation(const IndexBuffer * pInBuffer)
{
	int vertexNum = pInBuffer->size();
	if (vertexNum < 3) {
		return;
	}
	// generate N - 2 triangle
	for (int i = 0; i < vertexNum - 2; ++i) {
		clippedIndexBuffer.push_back((*pInBuffer)[0]);
		clippedIndexBuffer.push_back((*pInBuffer)[i + 1]);
		clippedIndexBuffer.push_back((*pInBuffer)[i + 2]);
	}
}

void RenderManager::DoProjection()
{
	int vertexNum = outVertexBuffer.size();
	// homogenouts divide
	for (int i = 0; i < vertexNum; i++) {
		double clipW = outVertexBuffer[i]->position.w;
		outVertexBuffer[i]->position /= clipW;
	}
	// project into screen space
	double resX = static_cast<double>(width);
	double resY = static_cast<double>(height);
	double centerX = resX / 2.0f;
	double centerY = resY / 2.0f;
	for (int i = 0; i < vertexNum; i++) {
		Vector4 & clipPosition = outVertexBuffer[i]->position;
		double clipX = clipPosition.x;
		double clipY = clipPosition.y;
		double doubleClipW = 2.0f * clipPosition.w;
		clipPosition.x = clipX * resX / doubleClipW + centerX;
		clipPosition.y = -clipY * resY / doubleClipW + centerY;
	}
}

void RenderManager::DoRasterization()
{
	ClearBuffer(PixelBuffer);
	PixelBuffer.resize(0, outVertexStride);
	int indexNum = clippedIndexBuffer.size();
	for (int i = 0; i < indexNum; i += 3) {
		// for each triangle, rearrange their vertexes' order as below
		// ============================================================
		//          top                            top
		//          /  \                          /  \
		//         /    \                        /    \
		//        /      \                      /      \
		//      mid       \        OR          /      mid
		//        \        \                  /        /
		//           \      \                /      /
		//              \    \              /    / 
		//               bottom            bottom
		//     "left" triangle             "right" triangle
		// ============================================================
		Vertex * pTop = outVertexBuffer[clippedIndexBuffer[i]];
		Vertex * pMid = outVertexBuffer[clippedIndexBuffer[i + 1]];
		Vertex * pBottom = outVertexBuffer[clippedIndexBuffer[i + 2]];
		if (pTop->position.y < pMid->position.y) {
			Vertex * pTemp = pTop;
			pTop = pMid;
			pMid = pTemp;
		}
		if (pMid->position.y < pBottom->position.y) {
			Vertex * pTemp = pMid;
			pMid = pBottom;
			pBottom = pTemp;
		}
		if (pTop->position.y < pMid->position.y) {
			Vertex * pTemp = pTop;
			pTop = pMid;
			pMid = pTemp;
		}
		if (pTop->position.y - pBottom->position.y < 0.5) {
			continue;
		}
		// ========== find out if it is "left" or "right" triangle ==========
		// ========= and set the vertexes of the two divided triangles =========
		// 0 for top, 1 for bottom, 2 for left, 3 for right
		Buffer triangleBuffer(4, outVertexStride);
		triangleBuffer.SetValue(pTop, 0);
		triangleBuffer.SetValue(pBottom, 1);
		triangleBuffer.SetValue(pMid, 2);
		double k = GetK(pTop->position.y, pMid->position.y, pBottom->position.y);
		triangleBuffer.SetValue(pTop, 3);
		triangleBuffer[3]->setValueByLI(pBottom, k);
		if (pMid->position.x > triangleBuffer[3]->position.x) {
			// "right" triangle
			triangleBuffer.SetValue(triangleBuffer[3], 2);
			triangleBuffer.SetValue(pMid, 3);
		}
		UpperTriangleRaster(triangleBuffer);
		BottomTriangleRaster(triangleBuffer);
	}
}

void RenderManager::UpperTriangleRaster(const Buffer & triangleBuffer)
{
	const Vertex * pTop = triangleBuffer[0];
	const Vertex * pLeft = triangleBuffer[2];
	const Vertex * pRight = triangleBuffer[3];
	if (pTop->position.y - pLeft->position.y < 0.25f) {
		// the triangle is too small to be painted
		return;
	}
	double leftSlope = GetSlopeXY(pTop->position, pLeft->position);
	double rightSlope = GetSlopeXY(pTop->position, pRight->position);

	double y = GetPixelCenterY(pTop->position);
	double yEnd = pLeft->position.y;
	// edgeVertexBuffer[0] point to left, edgeVertexBuffer[1] point to right
	Buffer edgeVertexBuffer(2, outVertexStride);

	while (y > yEnd) {
		GetEdgeVertex(pTop, pLeft, pRight, y, &edgeVertexBuffer);
		HorizontalRaster(pTop, edgeVertexBuffer, leftSlope, rightSlope);
		y--;
	}
}

void RenderManager::BottomTriangleRaster(const Buffer & triangleBuffer)
{
	const Vertex * pBottom = triangleBuffer[1];
	const Vertex * pLeft = triangleBuffer[2];
	const Vertex * pRight = triangleBuffer[3];
	if (pLeft->position.y - pBottom->position.y < 0.1f) {
		// the triangle is too small to be painted
		return;
	}
	double leftSlope = GetSlopeXY(pBottom->position, pLeft->position);
	double rightSlope = GetSlopeXY(pBottom->position, pRight->position);

	double y = GetPixelCenterY(pBottom->position);
	double yEnd = pLeft->position.y;
	// edgeVertexBuffer[0] point to left, edgeVertexBuffer[1] point to right
	Buffer edgeVertexBuffer(2, outVertexStride);

	while (y <= yEnd) {
		GetEdgeVertex(pBottom, pLeft, pRight, y, &edgeVertexBuffer);
		HorizontalRaster(pBottom, edgeVertexBuffer, leftSlope, rightSlope);
		y++;
	}
}

void RenderManager::GetEdgeVertex(const Vertex * pTop,
				   const Vertex * pLeft,
				   const Vertex * pRight,
				   const double y,
				   Buffer * outBuffer)
{
	double k = GetK(pTop->position.y, y, pLeft->position.y);
	outBuffer->SetValue(pTop, 0);
	(*outBuffer)[0]->setValueByLI(pLeft, k);
	outBuffer->SetValue(pTop, 1);
	(*outBuffer)[1]->setValueByLI(pRight, k);
}

void RenderManager::HorizontalRaster(const Vertex * pTop,
									 const Buffer & edgeVertexBuffer,
									 const double leftSlope,
									 const double rightSlope)
{
	const Vertex * pLeft = edgeVertexBuffer[0];
	const Vertex * pRight = edgeVertexBuffer[1];
	double pixelCenterX = GetPixelCenterX(pLeft->position);
	double pixelCenterY = GetPixelCenterY(pLeft->position);
	double k = GetK(pTop->position.y, pixelCenterY, pLeft->position.y);
	double beginX = LinearInterpolation(pTop->position.x, pLeft->position.x, k); 
	double endX = LinearInterpolation(pTop->position.x, pRight->position.x, k);
	// get the left most vertex that should be painted
	while (pixelCenterX <= beginX) {
		pixelCenterX++;
	}
	// do horizontal rasterization
	while (pixelCenterX <= endX) {
		PixelBuffer.push_back(pLeft);
		PixelBuffer.back()->setValueByLI(pRight, 
										   GetK(pLeft->position.x, pixelCenterX, pRight->position.x));
		PixelBuffer.back()->position.y = pixelCenterY;
		pixelCenterX++;
	}
}

void RenderManager::SetPixelShader(PixelShaderCallBack shaderCallBack)
{
	pixelShader = shaderCallBack;
}

void RenderManager::DoPixelShader()
{
	InilScreen();
	int PixelNum = PixelBuffer.size();
	char* pInPixel = PixelBuffer.data();
	size_t inPixelStride = PixelBuffer.GetStride();
	Color outPixel;

	for (int i = 0; i < PixelNum; ++i) {
		Vector4 inPosition = reinterpret_cast<RenderData *>(pInPixel)->position;
		if (inPosition.x >= 0.0f && inPosition.y >= 0.0f) {
			unsigned int y = static_cast<unsigned int>(inPosition.y);
			unsigned int x = static_cast<unsigned int>(inPosition.x);
			if (y < 600 && x < 800 && inPosition.z < depthBuffer.GetElem(y, x)) {
				pixelShader(reinterpret_cast<RenderData *>(pInPixel), &outPixel);
				screenBuffer.GetElem(y, x).SetByColor(outPixel);
				depthBuffer.GetElem(y, x) = inPosition.z;
			}
		}
		pInPixel += inPixelStride;
	}
}
void RenderManager::InilScreen()
{
	depthBuffer.InilByValue(FINITE);
	screenBuffer.InilByValue(backgroundColor);
}

void RenderManager::ClearBuffer(Buffer & buffer)
{
	buffer.clear();
}
void RenderManager::ClearBuffer(vector<int> & buffer)
{
	buffer.clear();
}
