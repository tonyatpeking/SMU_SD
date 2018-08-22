from zzz.shape_func import *
from random import randint, uniform, randrange

max_layers = 20
max_nodes = 1000


sun_angle = randint(0, 180)
contractible_angle = randint(10, 130)

angle1 = uniform(0.1, 2)
angle2 = uniform(-2, -0.1)

branch_iter = randint(6, 10)

zeros = [0, 0, 0]
ones = [1, 1, 1]


def Root():
    Cube([1, 1, 1])
    #AddNode(MainMod)
    #AddNode(BranchR, 1, branch_iter, 1, [0, -10, 0])
    AddNode( RandBranchR, 10, 0)

def MainMod():
    Cube([15, 1, 1])
    AddNode(WingBase, dir=1)
    AddNode(WingBase, dir=-1)

    for xpos in [-7, -5, -3, 3, 5, 7]:
        AddNode(SolarContractibleR, 8, [260, 0, 0], [xpos, 0, 0])

    AddNode(MechArm, [0, 20, -40], [0, 0, 0], 4, 0.15)
    AddNode(MechArm, [0, 180, -20], [-3, 0, 0], 3, 0.15)


def MechArm(r, t, length, width):
    Cube([width, length, width], [0, -1, 0])
    Translate(*t)
    Rotate(*r)
    AddNode(MechArmSeg, [0, 20, -80], [0, length, 0], 3, 0.1)


def MechArmSeg(r, t, length, width):
    Cube([width, length, width], [0, -1, 0])
    Translate(*t)
    Rotate(*r)


def WingBase(dir):
    Cube([7, 0.7, 0.7], [-dir, 0, 0])
    Translate(7.5 * dir, 0, 0)

    AddNode(SolarRod, [0, sun_angle, 0], [dir * 1, 0.5, 0])
    AddNode(SolarRod, [0, sun_angle, 0], [dir * 5, 0.5, 0])
    AddNode(SolarRod, [0, sun_angle, 180], [dir * 1, -0.5, 0])
    AddNode(SolarRod, [0, sun_angle, 180], [dir * 5, -0.5, 0])

    AddNode(SolarContractibleR, 8, [90, 0, 0], [dir * 0, 0, 0])
    AddNode(SolarContractibleR, 8, [90, 0, 0], [dir * 6, 0, 0])


def SolarRod(r, t, s=[1, 1, 1]):
    Cube([0.2, 11, 0.2], [0, -1, 0])
    Scale(*s)
    Translate(*t)
    Rotate(*r)
    AddNode(SolarPane, 1.5)
    AddNode(SolarPane, -1.5)


def SolarPane(dir):
    Cube([2.4, 10, 0.1], [dir, 0, 0])
    Translate(0, 6, 0)
    pass


def SolarContractibleR(iter, r=[0, 0, 0], t=[0, 0, 0], s=[1, 1, 1]):
    Cube([1.3, 1, 0.1], [0, -1, 0])
    Scale(*s)
    Translate(*t)
    Rotate(*r)
    iter -= 1
    if iter <= 0:
        return
    if iter % 2 == 0:
        dir = 1
    else:
        dir = -1
    child_rot = [contractible_angle * dir, 0, 0]
    child_t = [0, 1, 0]
    AddNode(SolarContractibleR, iter, child_rot, child_t)


# def GhettoCylinder(radius, height, r, t, width=3, count=8, thickness=0.05):
#     Cube([0.1, 0.1, 0.1], [0, -1, 0])
#     size = [width,height,thickness]
#     board_t = [0,0,radius]
#     for i in range(count):

#     AddNode( CylinderBoard, board_r, board_t, size )

def CylinderBoard(r, t, size):
    Cube(size, [0, -1, 0])
    Rotate(*r)
    Translate(*t)


def BranchR(size, iter, dir, offest=[0, 0, 0]):
    Cube([0.2*size, 3*size, 1*size], [0, -1, 0])
    Translate(0, 3*size, 0)
    Translate(*offest)
    Rotate(4, 0, 30 * dir)
    iter -= 1
    if iter <= 0:
        return
    AddNode(BranchR, size * 0.9, iter, angle1)
    AddNode(BranchR, size * 0.9, iter, angle2)


def RandBranchR(iter, parent_len):
    length = uniform(2, 10)
    width = uniform(0.05, 0.1)
    child_count = randint(1,3)
    Cube( [width, length, width], [0,-1,0] )
    pos = uniform(0,parent_len)
    Translate(0, pos, 0)
    rot = randrange(0,360,90)
    Rotate(90,0,0)
    Rotate(0,rot,0)

    lucky = randint(0,20)
    if lucky == 0:
        sv = length / 10
        s = [sv,sv,sv]
        AddNode(SolarRod, ones, ones, s)

    if iter <= 0:
        return
    for _ in range(child_count):
        iter_decay = randint(1,2)
        AddNode(RandBranchR, iter - iter_decay, length)