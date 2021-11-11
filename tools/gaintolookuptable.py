


f_in = open('gain.txt', 'r')
f_out_cpp = open('gain.cpp', 'w')
f_out_objc = open('gain.m', 'w')
f_out_java = open('Gain.java', 'w')

f_out_cpp.write("float thumbToGain[101][360][5] = {\n")
f_out_objc.write("float thumbToGain[101][360][5] = {\n")
f_out_java.write("public class Gain { \n public static double[][][] thumbToGain = {\n")

veryFirst = True
for i in range(0,101):

  if veryFirst:
    veryFirst = False
  else:
    f_out_cpp.write(",\n")
    f_out_objc.write(",\n")
    f_out_java.write(",\n")

  f_out_cpp.write("\t{\n")
  f_out_objc.write("\t{\n")
  f_out_java.write("\t{\n")


  first = True
  for j in range(0,360):
    line = f_in.readline()
    values = line.split(';')[0]
    values = values.split(',')

    r = values[0]
    theta = values[1]
    g1 = values[2]
    g2 = values[3]
    g3 = values[4]
    g4 = values[5]
    g5 = values[6]

    if first:
      first = False
    else:
      f_out_cpp.write(",\n")
      f_out_objc.write(",\n")
      f_out_java.write(",\n")

    f_out_cpp.write("   //  thumb="+r+","+theta+"\n")
    str = "\t\t{ "+g1+", "+g2+", "+g3+", "+g4+", "+g5+" }"
    f_out_cpp.write(str);
    f_out_objc.write("   //  thumb="+r+","+theta+"\n")
    f_out_objc.write(str);
    f_out_java.write(str);


  f_out_cpp.write("\n\t}")
  f_out_objc.write("\n\t}")
  f_out_java.write("\n\t}")


f_out_cpp.write("\n};")
f_out_objc.write("\n};")
f_out_java.write("\n};\n}")
