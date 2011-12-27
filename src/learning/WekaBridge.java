import java.io.*;
import java.util.*;
import weka.core.*;
import weka.classifiers.Classifier;

public class WekaBridge {
	static Classifier classifier;

  public static void main(String [] argv) {

    try {
      String[] opts = new String[0];
      classifier = Classifier.forName("weka.classifiers.trees.J48",opts);

      String toWekaName = argv[0];
      String fromWekaName = argv[1];
      //BufferedReader in = new Buffnew DataInputStream(new FileInputStream(toWekaName));
      BufferedReader in = new BufferedReader(new FileReader(toWekaName));
          //= new BufferedReader(new InputStreamReader(in));
      PrintStream out = new PrintStream(new FileOutputStream(fromWekaName));
      
      FastVector features = new FastVector();
      String line = in.readLine();
      while (!line.equals("@data")) {
        String []res = line.split(",");
        Attribute attr;
        if (res.length == 1)
          attr = new Attribute(res[0]);
        else {
          FastVector vals = new FastVector();
          for (int i = 1; i < res.length; i++) {
            vals.addElement(res[i]);
          }
          attr = new Attribute(res[0],vals);
        }
        features.addElement(attr);
        line = in.readLine();
      }
      Instances trainData = new Instances("train",features,0);
      trainData.setClassIndex(features.size()-1);
      Instances testData = new Instances(trainData,0);
      // build an initial classifier
			classifier.buildClassifier(trainData);

      while (true) {
        line = in.readLine();
        if ((line == null) || line.equals("exit")) {
          break;
        } else if (line.equals("add")) {
          Instance inst = readInst(in,trainData.numAttributes());
          trainData.add(inst);
        } else if (line.equals("train")) {
          classifier.buildClassifier(trainData);
        } else if (line.equals("classify")) {
          Instance inst = readInst(in,trainData.numAttributes());
          testData.add(inst);
          double[] distr = classifier.distributionForInstance(testData.lastInstance());
          for (int i = 0; i < distr.length; i++) {
            out.format("%f ",distr[i]);
          }
          out.format("%n");
        } else if (line.equals("")) {
          continue;
        } else {
          System.err.println("ERROR: unexpected line:");
          System.err.println(line);
          break;
        }
      }

      
      in.close();
      out.close();
      return;
    } catch (Exception e) {
      System.err.println("EXCEPTION:");
      System.err.println(e.toString());
      e.printStackTrace();
    }
  }
  public static Instance readInst(BufferedReader in, int numFeatures) throws java.io.IOException {
    double weight;
    double[] vals = new double[numFeatures];
    String line = in.readLine();
    String[] strVals = line.split(",");
    assert(strVals.length == numFeatures + 1);

    for (int i = 0; i < numFeatures; i++) {
      vals[i] = Double.parseDouble(strVals[i]);
    }
    weight = Double.parseDouble(strVals[numFeatures]);
    return new Instance(weight,vals);
  }
}

