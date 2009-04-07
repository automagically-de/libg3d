o = g3d.Object()

i = g3d.Image('default48.png')
m = g3d.Material()
m:setTexture(i)

o:addVertex(-1,  1, 0)
o:addVertex( 1,  1, 0)
o:addVertex( 1, -1, 0)
o:addVertex(-1, -1, 0)

f = g3d.Face(0, 1, 2, 3)
f:setMaterial(m)
f:addTexVertex(0, 0)
f:addTexVertex(1, 0)
f:addTexVertex(1, 1)
f:addTexVertex(0, 1)
o:addFace(f)

g3d.model:addObject(o)

