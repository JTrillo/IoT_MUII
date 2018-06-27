import java.io.IOException;
import java.util.StringTokenizer;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.PreparedQuery;
import com.google.appengine.api.datastore.Query;
import com.google.appengine.api.datastore.Query.Filter;
import com.google.appengine.api.datastore.Query.FilterOperator;
import com.google.appengine.api.datastore.Query.FilterPredicate;

@WebServlet(
		name = "AccessRequest",
		urlPatterns = {"/AccessRequest"}
)
public class AccessRequest extends HttpServlet{
	private static final long serialVersionUID = 1L;

	@Override
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		
		String email = request.getParameter("email");
		if(!email.equals("") && isGmail(email)) {
			DatastoreService ds = DatastoreServiceFactory.getDatastoreService();
			Filter filter = new FilterPredicate("email",FilterOperator.EQUAL, email);
			Query q = new Query("Requests").setFilter(filter);
			PreparedQuery pq = ds.prepare(q);
			if(pq.countEntities()==0) {
				Entity entity = new Entity("Requests");
				entity.setProperty("email", email);
				ds.put(entity);
				request.setAttribute("message", "Your access request has been successfully sended.");
			}else {
				request.setAttribute("message", "You have already sent a request.");
			}
		}
		
		request.getRequestDispatcher("/index.jsp").forward(request, response);
	}
	
	@Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		this.doGet(request, response);
	}
	
	//Check if email is a gmail's account
	private boolean isGmail(String email) {
		StringTokenizer st = new StringTokenizer(email, "@");
		if(st.countTokens()!=2) {
			return false;
		}
		st.nextToken();
		if(!st.nextToken().equals("gmail.com")) {
			return false;
		}
		return true;
	}
	
}
