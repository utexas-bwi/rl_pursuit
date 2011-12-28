import java.io.*;
import java.util.*;
import weka.core.*;
import weka.classifiers.Classifier;

public class WekaBridge {
	static Classifier classifier;

  public static void main(String [] argv) {

    try {
      // load packages
      weka.core.WekaPackageManager.loadPackages(false, false);
      // deal with command line options
      String toWekaName = argv[0];
      String fromWekaName = argv[1];
      String classifierName = argv[2];
      String[] opts = new String[argv.length-3];
      for (int i = 0; i < opts.length; i++)
        opts[i] = argv[i+3];
      classifier = (Classifier)Utils.forName(Classifier.class,classifierName,opts);

      //BufferedReader in = new Buffnew DataInputStream(new FileInputStream(toWekaName));
      BufferedReader in = new BufferedReader(new FileReader(toWekaName));
          //= new BufferedReader(new InputStreamReader(in));
      PrintStream out = new PrintStream(new FileOutputStream(fromWekaName));
      
      ArrayList<Attribute> features = new ArrayList<Attribute>();
      String line = in.readLine();
      while (!line.equals("@data")) {
        String []res = line.split(",");
        Attribute attr;
        if (res.length == 1)
          attr = new Attribute(res[0]);
        else {
          ArrayList<String> vals = new ArrayList<String>();
          for (int i = 1; i < res.length; i++) {
            vals.add(res[i]);
          }
          attr = new Attribute(res[0],vals);
        }
        features.add(attr);
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
          testData.delete();
          for (int i = 0; i < distr.length; i++) {
            out.format("%f ",distr[i]);
          }
          out.format("%n");
        } else if (line.equals("")) {
          continue;
        } else if (line.equals("save")) {
          line = in.readLine();
          System.out.println("SAVING " + line);
          saveModel(line);
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
    return new DenseInstance(weight,vals);
  }

	public static void saveModel(String filename) {
		try {
			weka.core.SerializationHelper.write(filename,classifier);
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
	}
}

