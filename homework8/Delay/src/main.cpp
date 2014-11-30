#include <iostream>
#include <vector>

int udiff( int a, int b ) {
  return (a > b) ? (a-b) : (b-a);
}

// Those are for the non DP algo
typedef std::vector<int> path_t;

struct path_and_delay{
  path_t path;
  int delay;
  int tot_delay;

  path_and_delay():
    delay(-1),
    tot_delay(-1)
  {}

  path_and_delay( const path_t& p, int d, int td ):
    path(p),
    delay(d),
    tot_delay(td)
  {}

};


// find the best sub path, knowing that the previous chosen point is p
// it is always either the points before points[0], or the points after points[n-1]
path_and_delay findBestSubPath( const path_t& points, int p){
  if( points.size() == 1 )
    return path_and_delay( points, 0, 0 );

  // best path with points[0] as an end point
  path_and_delay bestP0 = findBestSubPath( path_t( points.begin()+1, points.end()), points[0] );
  bestP0.delay += udiff( points[0], bestP0.path[ bestP0.path.size()-1 ] );
  bestP0.tot_delay += bestP0.delay;
  bestP0.path.push_back( points[0] );

  // best path with points[n-1] as an end point
  path_and_delay bestPn = findBestSubPath( path_t( points.rbegin()+1, points.rend()), points[points.size()-1] );
  bestPn.delay += udiff( points[points.size()-1], bestPn.path[ bestPn.path.size()-1 ] );
  bestPn.tot_delay += bestPn.delay;
  bestPn.path.push_back( points[points.size()-1] );


  if( bestP0.tot_delay + bestP0.delay + udiff( p, bestP0.path[ bestP0.path.size()-1 ]  )
      <= bestPn.tot_delay + bestPn.delay + udiff( p, bestPn.path[ bestPn.path.size()-1 ] ) )
    return bestP0;
  else
    return bestPn;
}


// find the best path, regardless of the previous chosen point
path_and_delay findBestPath( const path_t& points ) {

  if( points.size() == 1 ){
    return path_and_delay( points, 0, 0 );
  }

  // best path with points[0] as an end point
  path_and_delay bestP0 = findBestSubPath( path_t( points.begin()+1, points.end()), points[0] );
  bestP0.delay += udiff( points[0], bestP0.path[ bestP0.path.size()-1 ] );
  bestP0.tot_delay += bestP0.delay;
  bestP0.path.push_back( points[0] );

  // best path with points[n-1] as an end point
  path_and_delay bestPn = findBestSubPath( path_t( points.rbegin()+1, points.rend()), points[points.size()-1] );
  bestPn.delay += udiff( points[points.size()-1], bestPn.path[ bestPn.path.size()-1 ] );
  bestPn.tot_delay += bestPn.delay;
  bestPn.path.push_back( points[points.size()-1] );

  if( bestP0.tot_delay < bestPn.tot_delay )
    return bestP0;
  else
    return bestPn;
}

// those are for the DP algo
// now a path_t contains the indices to the elements in the points array
typedef std::vector<int> points_array_t;

// find the best sub path containing all the points between i and j (included)
path_and_delay findBestSubPathEndIsLeftDP(
  const points_array_t& global_points_array,
  path_and_delay * endIsLeftCached,
  path_and_delay * endIsRightCached,
  int i,
  int j );
path_and_delay findBestSubPathEndIsRightDP(
  const points_array_t& global_points_array,
  path_and_delay * endIsLeftCached,
  path_and_delay * endIsRightCached,
  int i,
  int j );

path_and_delay findBestSubPathEndIsLeftDP(
  const points_array_t& points,
  path_and_delay * endIsLeftCached,
  path_and_delay * endIsRightCached,
  int i,
  int j ) {

  unsigned n = points.size();
  bool cached = (endIsLeftCached[ i*n + j ].tot_delay != -1);
  if( cached ) {
    return endIsLeftCached[ i*n + j ];
  }else{//if( !cached ) {
    if( i == j ){
      path_t p;
      p.push_back(i);
      endIsLeftCached[ i*n + j ] = path_and_delay( p, 0, 0 );
      return endIsLeftCached[ i*n + j ];
    }

    // more than one point
    int p = points[ i-1 ];

    // best path with points[0] as an end point: end is on the left
    path_and_delay bestP0 = findBestSubPathEndIsLeftDP( points, endIsLeftCached, endIsRightCached, i+1, j );
    bestP0.delay += udiff( points[i], points[ bestP0.path[ bestP0.path.size()-1 ] ] );
    bestP0.tot_delay += bestP0.delay;
    bestP0.path.push_back( i );

    // best path with points[n-1] as an end point: end is on the right
    path_and_delay bestPn = findBestSubPathEndIsRightDP( points, endIsLeftCached, endIsRightCached, i, j-1 );
    bestPn.delay += udiff( points[j], points[ bestPn.path[ bestPn.path.size()-1 ] ] );
    bestPn.tot_delay += bestPn.delay;
    bestPn.path.push_back( j );


    if( bestP0.tot_delay + bestP0.delay + udiff( p, points[ bestP0.path[ bestP0.path.size()-1 ] ]  )
	<= bestPn.tot_delay + bestPn.delay + udiff( p, points[ bestPn.path[ bestPn.path.size()-1 ] ] ) ) {
      endIsLeftCached[ i*n + j ] = bestP0;
      return bestP0;
    }else{
      endIsLeftCached[ i*n + j ] = bestPn;
      return bestPn;
    }
  }
}

path_and_delay findBestSubPathEndIsRightDP(
  const points_array_t& points,
  path_and_delay * endIsLeftCached,
  path_and_delay * endIsRightCached,
  int i,
  int j ) {

  unsigned n = points.size();
  bool cached = (endIsRightCached[ i*n + j ].tot_delay != -1 );
  if( cached ) {
    return endIsRightCached[ i*n + j ];
  }else{//if( !cached ) {
    if( i == j ){
      path_t p;
      p.push_back(i);
      endIsRightCached[ i*n + j ] = path_and_delay( p, 0, 0 );
      return endIsRightCached[ i*n + j ];
    }

    int p = points[ j+1 ];

    // best path with points[0] as an end point: end is on the left
    path_and_delay bestP0 = findBestSubPathEndIsLeftDP( points, endIsLeftCached, endIsRightCached, i+1, j );
    bestP0.delay += udiff( points[i], points[ bestP0.path[ bestP0.path.size()-1 ] ] );
    bestP0.tot_delay += bestP0.delay;
    bestP0.path.push_back( i );

    // best path with points[n-1] as an end point: end is on the right
    path_and_delay bestPn = findBestSubPathEndIsRightDP( points, endIsLeftCached, endIsRightCached, i, j-1 );
    bestPn.delay += udiff( points[j], points[ bestPn.path[ bestPn.path.size()-1 ] ] );
    bestPn.tot_delay += bestPn.delay;
    bestPn.path.push_back( j );


    if( bestP0.tot_delay + bestP0.delay + udiff( p, points[ bestP0.path[ bestP0.path.size()-1 ] ]  )
	<= bestPn.tot_delay + bestPn.delay + udiff( p, points[ bestPn.path[ bestPn.path.size()-1 ] ] ) ) {
      endIsRightCached[ i*n + j ] = bestP0;
      return bestP0;
    }else{
      endIsRightCached[ i*n + j ] = bestPn;
      return bestPn;
    }
  }
}

// points are sorted along the axis
path_and_delay findBestPathDP( const points_array_t& points ) {
  if( points.size() == 1 ){
    path_t p;
    p.push_back(0);
    return path_and_delay( p, 0, 0 );
  }

  unsigned n = points.size();// number of points
  path_and_delay * endIsLeftCached = new path_and_delay[ n*n ] ;
  path_and_delay * endIsRightCached = new path_and_delay[ n*n ];

  // best path with points[0] as an end point
  path_and_delay bestP0 = findBestSubPathEndIsLeftDP( points, endIsLeftCached, endIsRightCached, 1, points.size()-1 );
  bestP0.delay += udiff( points[0], points[ bestP0.path[ bestP0.path.size()-1 ] ] );
  bestP0.tot_delay += bestP0.delay;
  bestP0.path.push_back( 0 );

  // best path with points[n-1] as an end point
  path_and_delay bestPn = findBestSubPathEndIsRightDP( points, endIsLeftCached, endIsRightCached, 0, points.size()-2 );
  bestPn.delay += udiff( points[points.size()-1], points[ bestPn.path[ bestPn.path.size()-1 ] ] );
  bestPn.tot_delay += bestPn.delay;
  bestPn.path.push_back( points.size()-1 );

  delete[] endIsLeftCached;
  delete[] endIsRightCached;

  if( bestP0.tot_delay < bestPn.tot_delay )
    return bestP0;
  else
    return bestPn;
}

int main( int argc, char** argv ) {

  unsigned points[] = { 0, 9, 10, 11, 20};
  path_t point_list(points, points + sizeof(points) / sizeof(unsigned));

  // non DP algo
  {
  path_and_delay best = findBestPath( point_list );
  std::cout<<"Total delay : "<<best.tot_delay<<std::endl;
  for( path_t::iterator it = best.path.begin();
       it != best.path.end();
       it++ )
    std::cout<<*it<<std::endl;
  }
  // DP algo
  {
  path_and_delay best = findBestPathDP( point_list );
  std::cout<<"Total delay : "<<best.tot_delay<<std::endl;
  for( path_t::iterator it = best.path.begin();
       it != best.path.end();
       it++ )
    std::cout<<point_list[ *it ]<<std::endl;
  }
}
