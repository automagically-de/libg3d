function Box(mat, w, h, d)
	obj = g3d.Object()
	obj:addVertex(-w / 2, -h / 2, -d / 2)
	obj:addVertex( w / 2, -h / 2, -d / 2)
	obj:addVertex( w / 2,  h / 2, -d / 2)
	obj:addVertex(-w / 2,  h / 2, -d / 2)
	obj:addVertex(-w / 2, -h / 2,  d / 2)
	obj:addVertex( w / 2, -h / 2,  d / 2)
	obj:addVertex( w / 2,  h / 2,  d / 2)
	obj:addVertex(-w / 2,  h / 2,  d / 2)

	obj.newMFace = function(self, mat, i1, i2, i3, i4)
		f = g3d.Face(i1, i2, i3, i4)
		f:setMaterial(mat)
		self:addFace(f)
	end

	obj:newMFace(mat, 0, 1, 2, 3)
	obj:newMFace(mat, 4, 5, 6, 7)
	
	obj:newMFace(mat, 0, 1, 5, 4)
	obj:newMFace(mat, 2, 3, 7, 6)
	
	obj:newMFace(mat, 1, 2, 6, 5)
	obj:newMFace(mat, 0, 3, 7, 4)

	return obj
end

for i = 0, 10 do
	for j = 0, 10 do
		for k = 0, 10 do
			m = g3d.Material()
			m:setColor(i / 10, j / 10, k / 10)

			b1 = Box(m, 2, 2, 2)

			matrix = g3d.Matrix()
			matrix:rotateXYZ(math.rad(i * 9), math.rad(j * 9), math.rad(k * 9))
			b1:transform(matrix)
		
			matrix = g3d.Matrix()
			matrix:translate(i * 5, j * 5, k * 5)
			b1:transform(matrix)

			g3d.model:addObject(b1)
		end
	end
end

