<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ page import="java.util.*" %>  
<%@ page import="org.json.JSONObject" %>  
<%@ page import="com.google.appengine.api.users.*" %>
<%@ page import="com.google.appengine.api.datastore.*" %>
<%@ page import="com.google.appengine.api.datastore.Query.CompositeFilter" %>
<%@ page import="com.google.appengine.api.datastore.Query.CompositeFilterOperator" %>
<%@ page import="com.google.appengine.api.datastore.Query.Filter" %>
<%@ page import="com.google.appengine.api.datastore.Query.FilterPredicate" %>
<%@ page import="com.google.appengine.api.datastore.Query.FilterOperator" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
	<head>
		<title>Vineyard Graph Panel</title>
		<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
		<!-- Latest compiled and minified CSS -->
		<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">  
		<style>
		html, body {
			height: 100%;
			padding: 0px;
			margin: 0px;
		}
		#mapbox{
			border: 1px solid #C4B797;
			display: inline-block;
			clear: left;
			width: 100%;
			text-align: left;
			padding-left: 1%;
			padding-right: 1%;
			padding-top: 1%;
			border-radius: 10px;
			margin-bottom: 2%;
		}
		#map {
			margin-bottom: 20px;
			height: 450px;
		}
		</style>
		<!-- Maps API -->
		<script src="https://maps.googleapis.com/maps/api/js?key=INSERT_YOUR_API_KEY"></script>
	</head>
	<body>
		<%
			List<Float> lats = new ArrayList<Float>();
			List<Float> lngs = new ArrayList<Float>();
			UserService userService = UserServiceFactory.getUserService();
			User user = userService.getCurrentUser();
			if(user != null){
				
				DatastoreService ds = DatastoreServiceFactory.getDatastoreService();
				Filter filter = new FilterPredicate("email",FilterOperator.EQUAL, user.getEmail());
				Query q = new Query("Users").setFilter(filter);
				PreparedQuery pq = ds.prepare(q);
				if(pq.countEntities()>0){
					
					pageContext.setAttribute("user", user);
		%>
				<h1 align="center"><%=user.getNickname() %>, welcome to the vineyard graph panel</h1><br>
		    	<h2 align="center">Here you can check your vineyard state</h2><br>
		    	
		    	<div class="row">
		    		<div class="col-md-offset-3 col-md-6">
				    	<div id="mapbox">
				    		<div id="map"></div>
				    	</div>
			    	</div>
		    	</div>
		    	
		    	<form action="${pageContext.request.contextPath}/requestGraph" method="post">
		    		<div class="form-group col-md-offset-3 col-md-6">
		    			<label for="node">Choose a node id:</label>
		    			<select id="node" class="form-control" name="node">
    	<%
		    		Query q2 = new Query("Vineyard2");
		    		PreparedQuery pq2 = ds.prepare(q2);
		    		Set<String> set_nodos = new HashSet<String>();
		    		Set<String> set_gateways = new HashSet<String>();
		    		for(Entity e : pq2.asIterable()){
		    			JSONObject json = new JSONObject(e.getProperty("description").toString());
		    			JSONObject identification = (JSONObject) json.get("identification");
		    			String gateway = identification.getString("gateway");
		    			String nodo = identification.getString("nodo");
		    			if(!set_gateways.contains(gateway)){
		    				JSONObject position = (JSONObject) json.get("position");
		    				lats.add(Float.parseFloat(position.getString("latitude")));
		    				lngs.add(Float.parseFloat(position.getString("longitude")));
		    				set_gateways.add(gateway);
		    			}
		    			if(!set_nodos.contains(nodo)){
		    				set_nodos.add(nodo);
		    				String toShow = "Node "+nodo+" (gateway "+gateway+")";
 		%>
							<option value="<%=nodo%>"><%=toShow %></option>
 		<%
		    			}
		    		}
    	%>
    					</select>
	    			</div>
	    			
			    	<div class="form-group col-md-offset-3 col-md-6">
			    		<label for="graph">Choose a graph:</label>
			    		<select id="graph" class="form-control" name="graph">
			    			<option value="TEMP">Temperature</option>
			    			<option value="HAIR">Humidity (air)</option>
			    			<option value="HGRO">Humidity (ground)</option>
			    			<option value="PRES">Pressure</option>
			    			<option value="BRIG">Brightness</option>
			    		</select>
		    		</div>
		    		
		    		<div>
		    			<button type="submit" class="btn btn-primary col-md-offset-5 col-md-2 col-sm-offset-5 col-sm-2">Show graph</button>
		    		</div>
	    		</form>
		<%		Filter filter2 = new FilterPredicate("admin",FilterOperator.EQUAL, "Yes");
				Filter compositefilter = new CompositeFilter(CompositeFilterOperator.AND, Arrays.asList(filter, filter2)); //We need a composed condition
				Query q3 = new Query("Users").setFilter(compositefilter);
				PreparedQuery pq3 = ds.prepare(q3);
				if(pq3.countEntities()>0){
		%>
				<div>
					<button type="button" class="btn btn-default col-md-offset-5 col-md-2 col-sm-offset-5 col-sm-2"><a href="management.jsp">Management</a></button>
				</div>	
		<%		} %>
				<div>
					<button type="button" class="btn btn-default col-md-offset-5 col-md-2 col-sm-offset-5 col-sm-2"><a href="<%= userService.createLogoutURL(request.getRequestURI()) %>">Logout</a></button>
				</div>	
		<%  	}else{%>
				<h1 align="center"><%=user.getNickname() %>, you have not access to the vineyard graph panel</h1><br>
				<h2 align="center">If you want to use the vineyard graph panel, please send us a request pressing 'Send request' button</h2>
				<c:if test="${not empty message}">
				    <div class="alert alert-info col-md-offset-3 col-md-6 col-sm-offset-3 col-sm-6">${message}</div>
				</c:if>
				<button type="button" class="btn btn-default col-md-offset-5 col-md-2 col-sm-offset-5 col-sm-2"><a href="AccessRequest?email=<%=user.getEmail() %>">Send request</a></button>
				<button type="button" class="btn btn-default col-md-offset-5 col-md-2 col-sm-offset-5 col-sm-2"><a href="<%= userService.createLogoutURL(request.getRequestURI()) %>">Logout</a></button>
		<%		}%>
		<%	}else{ %>
			<h1 align="center">Please log in to access to the vineyard graph panel</h1><br>
			<button type="button" class="btn btn-default col-md-offset-5 col-md-2 col-sm-offset-5 col-sm-2"><a href="<%= userService.createLoginURL(request.getRequestURI()) %>">Login</a></button>
		<%	}%>
		
		<!-- jQuery library -->
		<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
		<!-- Latest compiled JavaScript -->
		<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>
		<!-- Marker Cluster library-->
		<script src="https://developers.google.com/maps/documentation/javascript/examples/markerclusterer/markerclusterer.js"></script>
		
		<script>
			var locations = [];
		<%
			for(int i=0; i<lats.size(); i++){
		%>
			var lat = <%=lats.get(i)%>;
			var lng = <%=lngs.get(i)%>;
			var pos = {lat: lat, lng: lng};
			//console.log(lat, lng);
			//console.log(pos);
			locations.push(pos);
		<%
			}
		%>
			function initMap(){
				var map = new google.maps.Map(document.getElementById('map'), {
					center: locations[0],
					zoom: 8
				});
				
				var markers = locations.map(function(location, i){
					return new google.maps.Marker({
						position: location,
						label: 'G' + (i+1),
						map: map
					});
				});
				
				var markersCluster = new MarkerClusterer(map, markers,
					{imagePath: 'https://developers.google.com/maps/documentation/javascript/examples/markerclusterer/m'}
				);
			}
			
			initMap();
		</script>
	</body>
</html>
