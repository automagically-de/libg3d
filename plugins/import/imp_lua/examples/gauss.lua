minx = -20
minz = -20
maxx = 20
maxz = 20

nx = maxx - minx + 1
ox = -minx
oz = -minz

a = 5
b = 1 / nx

o = g3d.Object()
o:setName(string.format(
	"standard distribution (x: %d..%d, z: %d..%d, a = %.2f, b = %.4f ",
	minx, maxx, minz, maxz, a, b))
g3d.model:addObject(o)

m = g3d.Material()
m:setColor(0.2, 0.9, 0.4)

for z = minz, maxz do
	for x = minx, maxx do
		y1 = a * math.exp(-b * x * x)
		y2 = a * math.exp(-b * z * z)

		g3d.debug(y1 .. ", " .. y2)

		o:addVertex(x, y1 * y2, z)
		
		if x < maxx and z < maxz then
			f = g3d.Face(
				(oz + z) * nx + ox + x,
				(oz + z) * nx + ox + x + 1,
				(oz + z + 1) * nx + ox + x + 1,
				(oz + z + 1) * nx + ox + x)
			f:setMaterial(m)
			o:addFace(f)
		end
	end
end

