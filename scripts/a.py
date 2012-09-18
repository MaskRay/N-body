import OpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import time, sys, select, random, copy

class NBody(object):
    ypoz = 0
    width = height = length = 10000
    cs = []
    colors = []
    x = 0
    y = 0
    z = 0

    def __init__(self):
        glutDisplayFunc(self.display)
        glutReshapeFunc(self.reshape)
        glutKeyboardFunc(self.key_pressed)
        glutIdleFunc(self.idle)
        glEnable(GL_DEPTH_TEST)

    def idle(self):
        r, w, x = select.select([sys.stdin], [], [], 0)
        if r:
            try:
                n = input()
                if n > 0:
                    self.cs = [[float(x) for x in raw_input().split()] for i in xrange(n)]
                    for i in xrange(len(self.colors), len(self.cs)):
                        self.colors.append([random.random() for i in xrange(3)])
                    glutPostRedisplay()
            except:
                pass

    def drawSphere(self, center=(0,0,0), radius=1.0, sides=8):
        glPushMatrix()
        glTranslatef(*center)
        glutSolidSphere(radius, sides, sides)
        glPopMatrix()

    def reshape(self, w, h):
        glViewport(0, 0, w, h)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(80, float(w) / h, 0.1, 10000)
        glMatrixMode(GL_MODELVIEW)

    def drawRect(self, vs, ps):
        glBegin(GL_QUADS)
        for p in ps:
            glVertex3f(*vs[p])
        glEnd()

    def drawBox(self):
        glEnable(GL_BLEND)
        glDisable(GL_DEPTH_TEST)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

        v = []
        v.append([0, 0, 0])
        v.append([self.width, 0, 0])
        v.append([self.width, self.height, 0])
        v.append([0, self.height, 0])
        v.extend(copy.deepcopy(v))
        for i in xrange(4, 8):
            v[i][2] = self.length
        glColor4f(0, 1, 0, 0.6)
        self.drawRect(v, [0,1,2,3])
        self.drawRect(v, [4,5,6,7])
        self.drawRect(v, [0,3,7,4])
        self.drawRect(v, [1,2,6,5])
        self.drawRect(v, [0,1,5,4])
        self.drawRect(v, [2,3,7,6])

        glEnable(GL_DEPTH_TEST)
        glDisable(GL_BLEND);

    def display(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()

        #glRotatef(self.ypoz, 0, 1, 0);
        glTranslatef(self.x, self.y, self.z)
        gluLookAt(0, 0, self.length / 2, self.width, self.height,
                self.length / 2, 0, 1, 0)

        glShadeModel (GL_SMOOTH)
        glLightfv(GL_LIGHT0, GL_DIFFUSE, GLfloat_3(1,1,1))
        glLightfv( GL_LIGHT0, GL_POSITION, GLfloat_4(1,1,1,1) )
        glLightfv(GL_LIGHT0, GL_SPECULAR, GLfloat_4(1.0, 1.0, 1.0, 1.0))
        glLightfv(GL_LIGHT0, GL_AMBIENT, GLfloat_4(0.0, 1.0, 0.0, 1.0))
        glEnable(GL_LIGHTING)
        glEnable(GL_LIGHT0)
        #self.drawBox()
        for i, c in enumerate(self.cs):
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, self.colors[i]);
            self.drawSphere(c[:3], c[3])

        glutSwapBuffers()

    def key_pressed(self, key, *args):
        if key == 'q':
            self.ypoz -= 5
            glutPostRedisplay()
        elif key == 'e':
            self.ypoz += 5
            glutPostRedisplay()
        elif key == 'h':
            self.x += 500
            glutPostRedisplay()
        elif key == 'l':
            self.x -= 500
            glutPostRedisplay()
        elif key == 'k':
            self.z += 500
            glutPostRedisplay()
        elif key == 'j':
            self.z -= 500
            glutPostRedisplay()
        elif key == 'w':
            self.y -= 500
            glutPostRedisplay()
        elif key == 's':
            self.y += 500
            glutPostRedisplay()
        elif key == '\033':
            sys.exit()

if __name__ == "__main__":
    glutInit(sys.argv)
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH)
    glutInitWindowSize(800, 600);
    glutCreateWindow('N-body Simulator')
    NBody()
    glutMainLoop()
