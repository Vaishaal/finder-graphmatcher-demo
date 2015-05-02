from Tkinter import *
import Image, ImageDraw, ImageTk
import sys

"""paint.py: not exactly a paint program.. just a smooth line drawing demo."""

b1 = "up"
color = "white"
xold, yold = None, None
im = None
draw = None
query = None

def main():
  global im, draw, query
  root = Tk()
  query_im = Image.open(sys.argv[1])
  query = ImageTk.PhotoImage(query_im)
  width, height = query_im.size
  xscroll = Scrollbar(root, orient=HORIZONTAL)
  xscroll.grid(row=1, column=0, sticky=E+W)
  yscroll = Scrollbar(root, orient=VERTICAL)
  yscroll.grid(row=0, column=1, sticky=N+S)
  drawing_area = Canvas(root, width=min(1000, width), height=min(500, height),
                        xscrollcommand=xscroll.set, yscrollcommand=yscroll.set,
                        scrollregion=(0, 0, width, height))
  drawing_area.create_image((0, 0), anchor=NW, image=query)
  xscroll.config(command=drawing_area.xview)
  yscroll.config(command=drawing_area.yview)
  
  im = Image.new("L", (width, height), 0)
  draw = ImageDraw.Draw(im)
  def save():
    drawing_area.postscript(file="test.ps")
    im.save("test.png")
    root.destroy()
  root.protocol('WM_DELETE_WINDOW', save)
  drawing_area.grid(row=0, column=0, sticky=N+S+E+W)
  drawing_area.bind("<ButtonPress-1>", lambda event: b1down(event))
  drawing_area.focus_set()
  root.mainloop()

def b1down(event):
  x = event.widget.canvasx(event.x)
  y = event.widget.canvasy(event.y)
  print  y, x
  event.widget.create_oval(x - 2, y - 2, x + 2, y + 2, fill=color, outline=color)
  draw.ellipse((x - 2, y - 2, x + 2, y + 2), outline=color, fill=color)

if __name__ == "__main__":
  main()
