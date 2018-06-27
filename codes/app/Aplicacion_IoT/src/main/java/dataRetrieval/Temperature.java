package dataRetrieval;

import java.io.IOException;
import java.io.PrintWriter;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.SortedSet;
import java.util.TreeSet;

import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.PreparedQuery;
import com.google.appengine.api.datastore.Query;

import org.json.*;

@WebServlet(
	    name = "Temperature",
	    urlPatterns = {"/temperature"}
)
public class Temperature extends HttpServlet {
	private static final long serialVersionUID = 1L;
    
	@Override
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException {
		
		SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		String node = (String) request.getAttribute("node");
		
		DatastoreService ds = DatastoreServiceFactory.getDatastoreService();
		Query q = new Query("Vineyard2");
		PreparedQuery pq = ds.prepare(q);
		SortedSet<Date> timestamps = new TreeSet<Date>();
		List<String> temperatures = new ArrayList<String>();
		
		for(Entity e : pq.asIterable()) {
			JSONObject json = new JSONObject(e.getProperty("description").toString());
			JSONObject identification = (JSONObject) json.get("identification");
			String nodo = identification.getString("nodo");
			if(node.equals(nodo)) {
				String time = json.getString("time");
				JSONObject sensors = (JSONObject) json.get("sensors");
				String temp = sensors.getString("temperatura");
				
				Date date;
				try {
					date = formatter.parse(time);
					timestamps.add(date);
					temperatures.add(temp);
				} catch (ParseException e1) {
					e1.printStackTrace();
				}
			}
		}
		
		response.setContentType("text/html; charset=UTF-8");
		PrintWriter out = response.getWriter();
		
		out.println("<!DOCTYPE html>");
		out.println("<html>");
		out.println("<head>");
		out.println("<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>");
		out.println("<title>Temperature chart</title>");
		out.println("<!-- Plotly.js -->\r\n" + 
				"  <script src=\"https://cdn.plot.ly/plotly-latest.min.js\"></script>");
		out.println("<!-- Latest compiled and minified CSS -->\r\n" + 
				"		<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">");
		out.println("</head>");
		out.println("<body>");
		out.println("<div id=\"container\"></div>");
		out.println("<button type=\"button\" class=\"btn btn-default col-md-offset-5 col-md-2 col-sm-offset-5 col-sm-2\"><a href=\"/index.jsp\">Back</a></button>");
		out.println("<script>");
		out.println("var x_axis = [];");
		out.println("var y_axis = [];");
		int cont = 0;
		for(Date d : timestamps) {
			String aux = "'"+formatter.format(d)+"'";
			out.println("x_axis.push("+aux+");");
			out.println("y_axis.push("+temperatures.get(cont)+");");
			cont++;
		}
		/*for(int i=0; i<timestamps.size(); i++) {
			out.println("x_axis.push("+timestamps.get(i)+");");
			out.println("y_axis.push("+temperatures.get(i)+");");
		}*/
		out.println("var trace = {\r\n" + 
				"  x: x_axis,\r\n" + 
				"  y: y_axis,\r\n" + 
				"  mode: 'lines'\r\n" + 
				"};\r\n");
		out.println("var data = [trace];");
		out.println("var layout = {\r\n" + 
				"  title: 'Temperature chart',\r\n" + 
				"  xaxis: {\r\n" + 
				"    title: 'Date',\r\n" + 
				"    titlefont: {\r\n" + 
				"      family: 'Courier New, monospace',\r\n" + 
				"      size: 18,\r\n" + 
				"      color: '#7f7f7f'\r\n" + 
				"    }\r\n" + 
				"  },\r\n" + 
				"  yaxis: {\r\n" + 
				"    title: 'Temperature (°C)',\r\n" + 
				"    titlefont: {\r\n" + 
				"      family: 'Courier New, monospace',\r\n" + 
				"      size: 18,\r\n" + 
				"      color: '#7f7f7f'\r\n" + 
				"    }\r\n" + 
				"  }\r\n" + 
				"};");
		out.println("Plotly.newPlot('container', data, layout);");
		out.println("</script>");
		out.println("<!-- jQuery library -->\r\n" + 
				"		<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>\r\n" + 
				"		<!-- Latest compiled JavaScript -->\r\n" + 
				"		<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>");
		out.println("</body>");
		out.println("</html>");
		
	}
	
	@Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws IOException {
		this.doGet(request, response);
	}
}